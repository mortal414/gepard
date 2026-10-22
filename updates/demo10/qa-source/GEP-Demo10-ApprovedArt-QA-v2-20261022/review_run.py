"""Offline native console/BMP artifact checks. Never judges rendering or gameplay."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

SKINS = ["Camo", "Charcoal"]
VIEWS = ["front3q", "rear3q", "left", "right", "guns", "reargrille", "rearfar"]

def main(args):
    raw = args.log.read_bytes(); log = raw.decode("utf-8-sig", errors="replace")
    failures = []
    if "GEP_ART_SCENE terminal=NOT_TESTED" in log: failures.append("scene reported NOT_TESTED")
    if len(re.findall(r"GEP_ART_SCENE terminal=CAPTURE_REQUESTS_COMPLETE actualFunctionalSubjects=2 completedViews=14", log)) != 1:
        failures.append("missing or duplicate successful request terminal")
    for skin in SKINS:
        ends = re.findall(r"GEP_ART_NATIVE_END skin=" + skin + r" parts=\d+ complete=(\w+)", log)
        if len(ends) != 1 or ends[0] not in ("1", "true"): failures.append(f"incomplete/duplicate native readout: {skin}")
        models = re.findall(r"GEP_ART_NATIVE_MODEL skin=" + skin + r" modelIndex=(\d+) count=(\d+) expected=([^\r\n]+)", log)
        if len(models) != 8 or {int(row[0]) for row in models} != set(range(8)) or any(int(row[1]) != 1 for row in models):
            failures.append(f"eight native model registrations not exactly once: {skin}")
    expected_models = ['{578CF9C31193CC94}Assets/ArtApproved20261022/Mobility/MobilityRig.xob', '{4D7043CCA2E8BD79}Assets/ArtApproved20261022/Gepard/GepardTurretRig.xob', '{C6D9B89642E424E3}Assets/ArtV11/Gepard/RemainingExterior669/RightStorage669.xob', '{2A7F69D8A7902DE2}Assets/ArtApproved20261022/Gepard/GepardGunLeft.xob', '{3772144B4AA90B1E}Assets/ArtApproved20261022/Gepard/GepardGunRight.xob', '{B621A5631346B7A3}Assets/ArtApproved20261022/Gepard/ReferenceFittings668/CoverLeft668.xob', '{418DE4B4FC8541A0}Assets/ArtApproved20261022/Gepard/ReferenceFittings668/CoverRight668.xob', '{C23AF0E6361BFB2C}Assets/ArtApproved20261022/Gepard/RearCover671/RightShieldCover671.xob']
    for skin in SKINS:
        records = re.findall(r"GEP_ART_NATIVE_MODEL skin=" + skin + r" modelIndex=(\d+) count=(\d+) expected=([^\r\n]+)", log)
        if {int(row[0]): row[2].strip() for row in records} != dict(enumerate(expected_models)):
            failures.append(f"exact approved model owner resources differ: {skin}")
    images = []
    for skin in SKINS:
        for view in VIEWS:
            captures = re.findall(r"GEP_ART_NATIVE_CAPTURE skin=" + skin + r" view=" + view + r" requested=(\w+)", log)
            if len(captures) != 1 or captures[0] not in ("1", "true"):
                failures.append(f"capture request missing/duplicate/failed: {skin}/{view}")
            readbacks = re.findall(r"GEP_ART_NATIVE_VIEW_READBACK skin=" + skin + r" view=" + view + r" worldMs=([^\s]+) timeScale=([^\s]+)", log)
            if len(readbacks) != 1 or float(readbacks[0][1]) != 1: failures.append(f"camera readback missing/duplicate/wrong timescale: {skin}/{view}")
            candidates = list(args.profile.rglob(f"artnative-demo10-new-{skin}-{view}.bmp"))
            if len(candidates) != 1:
                failures.append(f"exact one BMP required: {skin}/{view}, found{len(candidates)}"); continue
            path = candidates[0]; data = path.read_bytes()
            valid = False; width = 0; height = 0; bits = 0
            if len(data) >= 54 and data[:2] == b"BM":
                offset = struct.unpack_from("<I", data, 10)[0]
                dib = struct.unpack_from("<I", data, 14)[0]
                width, height = struct.unpack_from("<ii", data, 18)
                planes, bits = struct.unpack_from("<HH", data, 26)
                compression = struct.unpack_from("<I", data, 30)[0]
                stride = ((width * bits + 31) // 32) * 4
                valid = dib >= 40 and width > 0 and height != 0 and planes == 1 and bits in (24,32) and compression == 0 and offset >= 54 and offset + stride * abs(height) <= len(data)
            if not valid: failures.append(f"unsupported or incomplete BMP: {skin}/{view}")
            images.append({"skin": skin, "view": view, "path": str(path.resolve()), "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest(),
                "bmpWidth": width, "bmpHeight": abs(height), "bits": bits, "headerAndDataLengthSupported": valid,
                "framingMaterialsGeometryAcceptance": "PENDING_VISUAL_REVIEW"})
    result = {"status": "READOUT_AND_IMAGE_FILES_PRESENT_VISUAL_REVIEW_PENDING" if not failures else "INCOMPLETE_OR_FAILED_NATIVE_EVIDENCE",
        "failures": failures, "console": str(args.log.resolve()), "consoleSha256": hashlib.sha256(raw).hexdigest(), "images": images,
        "nativeCompile": "REQUIRES_SEPARATE_PARENT_RECEIPT", "processExit": "REQUIRES_SEPARATE_PARENT_RECEIPT",
        "limits": "BMP headers/data lengths and console assertions only. Does not prove unobstructed target framing, screenshot correspondence, loaded model bytes, GPU materials, geometry, bone parenting or gameplay. Retain warning for deleted tire roles.",
        "artAcceptance": "PENDING_USER", "newPadsBound": False}
    with args.output.open("x", encoding="utf-8") as output:
        json.dump(result, output, indent=2); output.write("\n")
    print(json.dumps({"status": result["status"], "images": len(images), "failures": failures}))
    return 0 if not failures else 2

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--log", type=Path, required=True)
    parser.add_argument("--profile", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    raise SystemExit(main(parser.parse_args()))
