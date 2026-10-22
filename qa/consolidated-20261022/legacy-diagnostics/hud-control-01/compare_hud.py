"""C-only extension; does not modify prior report/seals or D: artifacts."""
from pathlib import Path
from collections import Counter
import json,hashlib,re,datetime
O=Path(__file__).resolve().parent
P=O.parent
R=Path(r'D:\ArmaProjects\Gepard-Consolidated-20261022')
ROOT=R/'native-runs/hud-render-control-01'
CONSOLE=ROOT/'hud-render-startup/native-run/logs/console.log'
def hash_bytes(b):return hashlib.sha256(b).hexdigest()
def pin(p):
    b=p.read_bytes();return {'path':str(p),'bytes':len(b),'sha256':hash_bytes(b)}
prior={str(p):pin(p) for p in P.iterdir() if p.is_file()}
raw=CONSOLE.read_bytes();lines=raw.decode('utf-8',errors='replace').splitlines()
history=json.loads((O/'HISTORY-SCAN.json').read_text())
exact="Unknown class 'SCR_WidgetExportRuleRoot' at offset 282(0x11a)"
resource='{1C71B463B3B66BAB}UI/layouts/Menus/MainMenu/IntroSplashScreen.layout'
def gui_matches(rows):
    hits=[]
    for row in rows:
        for hit in row['hits']:
            if exact in hit['raw'] and any('Layout load @"'+resource+'"' in x['raw'] for x in hit['context']):hits.append({'path':row['path'],'consoleSha256':row['sha256'],'line':hit['line'],'raw':hit['raw'],'context':hit['context']})
    return hits
matches=gui_matches(history['logs'])
currentGui=[{'line':i+1,'raw':l,'context':[{'line':j+1,'raw':lines[j]} for j in range(max(0,i-3),min(len(lines),i+3))]} for i,l in enumerate(lines) if exact in l]
assert len(currentGui)==1
assert any(resource in r['raw'] for r in currentGui[0]['context'])
sourceScan=[];fieldHits=[]
for root in [R/'addon',R/'native-stage/gepard',R/'native-stage/hudQA']:
    rows=[]
    for p in sorted(root.rglob('*')):
        if not p.is_file() or p.suffix.lower() not in ['.c','.layout','.conf','.et','.gproj']:continue
        b=p.read_bytes();text=b.decode('utf-8',errors='replace')
        rows.append({'path':str(p),'sha256':hash_bytes(b)})
        for i,line in enumerate(text.splitlines()):
            if any(token in line for token in ['SCR_WidgetExportRuleRoot','IntroSplashScreen.layout','1C71B463B3B66BAB']):fieldHits.append({'path':str(p),'line':i+1,'raw':line})
    sourceScan.append({'root':str(root),'textSourceFiles':len(rows),'files':rows})

# Reuse the prior immutable seven-error signature evidence by reading, never rewriting it.
old=json.loads((P/'COMPARISON.json').read_text())
worlds=[];enclosing=None
pat=re.compile(r"Unknown (keyword/data|class) '([^']+)' at offset (\d+)\((0x[0-9a-fA-F]+)\)")
for i,line in enumerate(lines):
    if 'Entity prefab load @' in line:enclosing=line.split('Entity prefab load @',1)[1].strip().strip('"')
    if 'WORLD' in line and '(E)' in line:
        m=pat.search(line);sig=[m[1],m[2],int(m[3]),enclosing] if m else [None,None,None,enclosing]
        original=[e for e in old['startup']['errors'] if e['signature']==sig]
        worlds.append({'line':i+1,'raw':line,'enclosingPrefab':enclosing,'signature':sig,'priorStartupMatchingLines':[e['line'] for e in original],'classification':'LEGACY_EXACT_BASELINE_MATCH' if original else 'NEW_OR_UNEXPLAINED'})
assert len(worlds)==7 and all(w['classification']=='LEGACY_EXACT_BASELINE_MATCH' for w in worlds)
plan=json.loads((ROOT/'plan.json').read_text());summary=json.loads((ROOT/'summary.json').read_text());result=json.loads((ROOT/'hud-render-startup/result.json').read_text());review=json.loads((ROOT/'hud-render-startup/native-review.json').read_text())
recipePins=[pin(p) for p in [ROOT/'plan.json',ROOT/'summary.json',ROOT/'hud-render-startup/result.json',ROOT/'hud-render-startup/native-review.json',CONSOLE]]
allErrors=[{'line':i+1,'raw':l} for i,l in enumerate(lines) if '(E):' in l]
report={'auditUtc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'scope':'C-only separate extension; no D: mutation, collector/whitelist change or native action','controlConsole':pin(CONSOLE),'guiSignature':{'channel':'GUI','kind':'Unknown class','class':'SCR_WidgetExportRuleRoot','offsetDecimal':282,'offsetHex':'0x11a','enclosingResource':resource},'currentGui':currentGui,'exactHistoricalMatches':matches,'historicalLogsScanned':len(history['logs']),'logsWithExactGuiSignature':len(matches),'guiClassificationProposal':'LEGACY_EXACT_BASELINE_MATCH_IN_STOCK_INTRO_SPLASH_LAYOUT','worldErrors':worlds,'sourceScan':sourceScan,'newQAOrGepardDefinitionsOrReferences':fieldHits,'nativeMarkers':review.get('markers'),'strictRunStatusRetained':summary['status'],'strictNativeErrorsStatusRetained':result['assertions']['native.errors']['status'],'recordedAssertions':{k:v['status'] for k,v in result['assertions'].items()},'allRawErrorLinesOutsideClassificationAlsoRetained':allErrors,'recipePins':recipePins,'priorReportPins':prior,'limits':['No broad Unknown class waiver is proposed; matching requires exact channel, class, offset and stock layout GUID/name.','Literal stock layout payload and historical full PAK byte identity were not decoded/established; attribution uses exact native load evidence and repeated baseline signature.','Layout load precedes Game-script compilation; this is stock startup parsing, not execution of a new QA component. Absence of these identifiers in scanned QA/Gepard sources is corroboration.','Resource leak messages during shutdown are retained separately and receive no classification here.','Reported ADS/data/capture assertion passes are not new visual/physical/gameplay acceptance. Native.errors and run FAIL remain unchanged.']}
(O/'COMPARISON-HUD.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')

md=['# HUD control-01 GUI diagnostic comparison','',
'**Proposal: the exact `GUI(E) Unknown class SCR_WidgetExportRuleRoot` offset 282 is a legacy stock-intro-layout match.** The seven WORLD(E) lines also match the previously documented baseline signatures. The recorded run and `native.errors` remain **FAIL**. No whitelist, collector, source, native session or prior report was changed.','',
f"Current console: `{CONSOLE}`; SHA256 `{report['controlConsole']['sha256']}`.",'',
'## Direct resource and ordering evidence','',
'Current lines 110–111 load `{1C71B463B3B66BAB}UI/layouts/Menus/MainMenu/IntroSplashScreen.layout`; line 112 reports the unknown class at decimal 282 / hex 0x11a. Game-script compilation begins at line 113. This is a stock main-menu splash resource parsed before the later QA scene/player/ADS/data/capture stages, not an error nested in a new QA widget/component resource.','',
'No `SCR_WidgetExportRuleRoot`, `IntroSplashScreen.layout` or its GUID occurs in the scanned Gepard production source, staged Gepard source or staged HUD QA `.c/.layout/.conf/.et/.gproj` files. The source-file hashes and counts are retained in `COMPARISON-HUD.json`. This negative source scan supports the direct stock-layout load evidence; it does not replace a native resource trace.','',
'## Exact historical comparisons','',
'Every listed match has the same GUI error class, decimal/hex offset and immediately enclosing stock layout GUID/name.','',
'| Historical native console | Error line | SHA256 |','| --- | --- | --- |']
for m in matches:md.append(f"| `{m['path']}` | {m['line']} | `{m['consoleSha256']}` |")
md+=['',f"Bounded scan: {len(history['logs'])} logs, {len(matches)} exact matches. Compile-only logs without splash-layout loads are retained as non-applicable comparisons, not evidence that the issue disappeared. `HISTORY-SCAN.json` preserves every scanned path/hash and matching context.",'',
'Released source/package consoles show the layout at 106–107 and the exact error at 108. The exact uploaded 1.0.2 console shows layout 109–110 and error 111. Earlier evening optic-render/new-optic-user runs and historical human HUD/driving runs show layout 103–104 and error 105. Their contexts and hashes are retained independently.','',
'## WORLD and other diagnostic boundaries','',
'The current seven WORLD(E) lines are 166, 173–174 and 181–184. They match the prior seven signatures by keyword, offset and enclosing stock M151A2/BRDM2/BTR70 prefab GUID/name. The separate original report and its seals remain untouched.','',
'Shutdown resource-leak diagnostics are preserved separately and not classified or waived by this report. Other classes, offsets, resources or channels must be treated as new/unexplained until individually supported. No general Unknown-class exception is justified.','',
'## Acceptance boundary','',
'The control run records fixture/world/ADS/initial-data/capture checks as PASS. Those remain the original limited claims; this source-only comparison adds no visual readability, physical interaction, transition or gameplay acceptance. Both strict native.errors and aggregate run FAIL remain unmodified. Legacy matching shows occurrence predates the new HUD QA component; it does not prove harmlessness.','',
'Missing evidence: decoded stock-layout field payload and historical full stock PAK hashes. They are not fabricated or required to call this an exact retained native-signature match, but no full stock-binary identity claim is made.','',
'Reproduce by running `scan_history.py`, then `compare_hud.py`, with Python 3.11. Both scripts write only inside this new C: subfolder.','']
(O/'REPORT.md').write_text('\n'.join(md),encoding='utf-8')
changed=[path for path,p in prior.items() if pin(Path(path))['sha256']!=p['sha256']]
logChanged=[m['path'] for m in history['logs'] if pin(Path(m['path']))['sha256']!=m['sha256']]
sourceChanged=[p['path'] for root in sourceScan for p in root['files'] if pin(Path(p['path']))['sha256']!=p['sha256']]
verification={'status':'PASS' if not changed+logChanged+sourceChanged else 'INPUT_CHANGED','priorReportFilesPreserved':len(prior),'changedPriorReportFiles':changed,'historicalLogPinsRechecked':len(history['logs']),'changedLogs':logChanged,'changedScannedSourceFiles':sourceChanged,'productionWrites':False,'collectorChanges':False,'nativeActions':False,'strictRunStatus':'FAIL','strictNativeErrors':'FAIL'}
(O/'VERIFICATION.json').write_text(json.dumps(verification,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'report':str(O/'REPORT.md'),'exactLegacyGUI':True,'matchingHistoricalConsoles':len(matches),'sourceIdentifierHits':len(fieldHits),'worldErrorCount':len(worlds),**verification},indent=2))
