"""Offline assessment of independently normalized native observations; never launches.

Input: a JSON array. Each record requires case, kind and evidence (relative paths
to actual native logs/captures). Evidence is hashed in the report. Missing native
observations remain NOT_TESTED. This tool does not import a source model or turn a
table height/telemetry record into an impact or visual pass.
"""
import argparse
import hashlib
import json
import math
from pathlib import Path


def vector(value):
    if not isinstance(value, list) or len(value) != 3:
        raise ValueError("expected a three-axis vector")
    result = [float(v) for v in value]
    if not all(math.isfinite(v) for v in result):
        raise ValueError("nonfinite vector")
    return result


def number(value):
    if isinstance(value, bool):
        raise ValueError("boolean is not a measurement")
    result = float(value)
    if not math.isfinite(result):
        raise ValueError("nonfinite measurement")
    return result


def evaluate(record, root):
    result = {"case": record.get("case"), "status": "NOT_TESTED", "evidence": []}
    if not isinstance(record.get("evidence"), list) or not record["evidence"]:
        result["reason"] = "actual native evidence absent"
        return result
    for name in record["evidence"]:
        path = (root / name).resolve()
        if not path.is_relative_to(root.resolve()) or not path.is_file():
            raise ValueError("missing/escaping native evidence")
        result["evidence"].append({"path": name, "sha256": hashlib.sha256(path.read_bytes()).hexdigest()})
    kind = record["kind"]
    checks = {}
    if kind == "range_valid":
        for field in ("state", "valid", "ageSeconds", "displayMatchesSample", "targetSurfaceVerified"):
            if field not in record:
                raise ValueError("missing observation: " + field)
        camera, point, forward = (vector(record[k]) for k in ("cameraOrigin", "samplePoint", "cameraForward"))
        delta = [p-c for p, c in zip(point, camera)]
        length = math.sqrt(sum(v*v for v in delta))
        axis = math.sqrt(sum(v*v for v in forward))
        if length <= 0 or axis <= 0:
            raise ValueError("zero range/axis")
        distance = number(record["sampleMetres"])
        dot = sum(v*f for v, f in zip(delta, forward)) / (length*axis)
        angle = math.degrees(math.acos(max(-1, min(1, dot))))
        # Geometric consistency only: the supplied camera must be captured at
        # ranging time, not a later snapshot after moving the sight.
        checks["slantMetresConsistent"] = abs(length-distance) <= 0.05
        checks["sampleOnCapturedRay"] = angle <= 0.02
        checks["validState"] = record.get("state") == 1 and record.get("valid") is True
        checks["freshSample"] = 0 <= number(record["ageSeconds"]) <= 0.5
        result["metrics"] = {"geometricMetres": length, "rayErrorDegrees": angle}
        # Pixel inspection is a separate required observation, not derived
        # from the source formula for the widget.
        checks["displayMatchesActualSample"] = record.get("displayMatchesSample") is True
        checks["independentTargetGeometry"] = record.get("targetSurfaceVerified") is True
    elif kind == "range_clear":
        for field in ("valid", "state", "expectedState", "oldNumericDisplayAbsent"):
            if field not in record:
                raise ValueError("missing observation: " + field)
        checks["invalidRead"] = record.get("valid") is False
        checks["expectedState"] = record.get("state") == record.get("expectedState")
        checks["noOldNumericDisplay"] = record.get("oldNumericDisplayAbsent") is True
    elif kind == "physical_correction":
        for field in ("impactFromFiredProjectile", "impactInsideDeclaredTarget", "manualControlRestored", "directPoseSettersUsed"):
            if field not in record:
                raise ValueError("missing observation: " + field)
        checks["actualBarrelMoved"] = abs(number(record["afterPitchDegrees"])-number(record["beforePitchDegrees"])) > 0.02
        checks["realImpactCaptured"] = record.get("impactFromFiredProjectile") is True
        checks["impactInsideDeclaredTarget"] = record.get("impactInsideDeclaredTarget") is True
        checks["manualControlRestored"] = record.get("manualControlRestored") is True
        checks["noPoseSetterEvidence"] = record.get("directPoseSettersUsed") is False
    elif kind == "stabilization":
        for field in ("actualHullMotion", "manualOverrideAndExit", "directPoseSettersUsed"):
            if field not in record:
                raise ValueError("missing observation: " + field)
        checks["actualHullMotion"] = record.get("actualHullMotion") is True
        checks["worldSightErrorWithinDeclaredLimit"] = number(record["maxWorldLOSErrorDegrees"]) <= number(record["declaredLimitDegrees"])
        checks["manualOverrideAndExit"] = record.get("manualOverrideAndExit") is True
        checks["noPoseSetterEvidence"] = record.get("directPoseSettersUsed") is False
    else:
        raise ValueError("unknown observation kind")
    result["checks"] = checks
    result["status"] = "PASS" if all(checks.values()) else "FAIL"
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--observations", type=Path, required=True)
    parser.add_argument("--evidence-root", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    records = json.loads(args.observations.read_text(encoding="utf-8-sig"))
    if not isinstance(records, list):
        raise ValueError("observations must be an array")
    results = []
    for record in records:
        try:
            results.append(evaluate(record, args.evidence_root))
        except (ValueError, KeyError, TypeError, OSError) as error:
            results.append({"case": record.get("case"), "status": "NOT_TESTED", "reason": str(error)})
    report = {"status": "NOT_TESTED" if not results or any(r["status"] == "NOT_TESTED" for r in results) else "FAIL" if any(r["status"] == "FAIL" for r in results) else "PASS", "scope": "assessment of supplied observations only; no whole-GEP acceptance", "results": results}
    args.output.write_text(json.dumps(report, indent=2)+"\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "observations": len(results)}))


if __name__ == "__main__":
    main()
