"""Source-closure/safety audit only. Does not compile Enforce or run Reforger."""
import argparse
import hashlib
import json
import re
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument("--source", type=Path, required=True)
parser.add_argument("--output", type=Path, required=True)
args = parser.parse_args()
here = Path(__file__).resolve().parent
source = args.source
game = source / "addon/scripts/Game/GEP"
names = ["GEP_FireControl.c", "GEP_FireControlGeometry.c", "GEP_Range676.c", "GEP_GunnerSnapshot583.c", "GEP_Presentation680.c", "GEP_GunStatus.c"]
files = {name: (game/name).read_text(encoding="utf-8-sig") for name in names}
helper_path = here / "fixture/GEP_EveningRangeProbe.c"
helper = helper_path.read_text(encoding="utf-8-sig")
# Strip comments before checking calls; source text in comments is not execution.
code = re.sub(r"//[^\n]*|/\*.*?\*/", "", helper, flags=re.S)
scenario = json.loads((here/"scenarios.json").read_text(encoding="utf-8-sig"))
checks = {
    "releasedAssistanceHardDisabled": "m_Enabled=false;" in files["GEP_FireControl.c"] and "m_DirectExperiment=false;" in files["GEP_FireControl.c"].split("override void OnPostInit", 1)[1],
    "helperHasNoActuationCalls": not re.search(r"\b(?:Set\w+|Spawn\w+|Delete\w+|Launch|Fire|AddActionListener|RemoveActionListener|RPC\w*|Rpc|CallLater|ActivateContext)\s*\(", code),
    "helperUnboundNoCallbacks": "OnPostInit" not in code and "EOnFrame" not in code and "EntityEvent" not in code,
    "tableOptInDefaultsFalse": "bool queryTable = false" in code and "if(!queryTable || !presentation || !operational || !valid)return;" in code,
    "tableResultCannotClaimSolution": "aimSolution=0" in code,
    "usesExistingRangeReadAPI": "bool Read676(" in files["GEP_Range676.c"] and "range.Read676(" in code,
    "usesExistingEligibilityAPI": "static bool Evaluate(" in files["GEP_Presentation680.c"] and "static bool Evaluate679(" in files["GEP_GunnerSnapshot583.c"],
    "usesPreviouslyAuthoredTableAPI": "BallisticTable.GetAimHeightOfNextProjectile(" in files["GEP_FireControl.c"] and "BallisticTable.GetAimHeightOfNextProjectile(" in code,
    "helperDelimitersBalanced": code.count("{") == code.count("}") and code.count("(") == code.count(")"),
    "releaseScenariosAndDevelopmentGatesSeparate": len(scenario["releaseControl"]) == 5 and len(scenario["developmentOnly"]) == 3,
    "pendingNativeLedgerEmpty": json.loads((here/"observations-pending.json").read_text(encoding="utf-8-sig")) == [],
}
manifest = [{"path": str((game/name).resolve()), "sha256": hashlib.sha256((game/name).read_bytes()).hexdigest()} for name in names]
deliverables = [{"path": path.relative_to(here).as_posix(), "sha256": hashlib.sha256(path.read_bytes()).hexdigest()} for path in sorted(here.rglob("*")) if path.is_file() and path.suffix in {".py", ".c", ".json", ".md"} and path.name not in {args.output.name, "STATIC-RESULT.json", "PENDING-NATIVE-ANALYSIS.json"}]
result = {"status": "PASS" if all(checks.values()) else "FAIL", "scope": "source contract only; helper native compile and all gameplay NOT_TESTED", "checks": checks, "sourceManifest": manifest, "deliverableManifest": deliverables}
args.output.write_text(json.dumps(result, indent=2)+"\n", encoding="utf-8")
print(json.dumps({"status": result["status"], "checks": len(checks), "sourceFiles": len(manifest), "native": "NOT_TESTED"}))
raise SystemExit(0 if all(checks.values()) else 1)
