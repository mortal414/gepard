"""Synthetic parser/false-positive tests only; these are not native smoke evidence."""
import json,struct,tempfile,unittest
from pathlib import Path
from analyze_wcs_smoke import analyze,SCOPE

class AnalyzerTests(unittest.TestCase):
 def setUp(self):
  self.tmp=tempfile.TemporaryDirectory(prefix='GEP015-analyzer-synthetic-');self.root=Path(self.tmp.name);self.profile=self.root/'profile';self.profile.mkdir();(self.root/'logs').mkdir()
 def tearDown(self):self.tmp.cleanup()
 def fixture(self,mode='single',stationary=False,world_zero=False):
  count=1 if mode=='single' else 8;after=8-count
  self.lines=['SYNTHETIC_TEST_ONLY','GEP215_WCS_PREFLIGHT crew=settled seatName=gep015_gunner ammo=8 capacity=8',f'GEP215_WCS_PROGRAM callback=RunProgramFromController_S requested=1 shotsExpected={count} physicalInput=NOT_TESTED']
  for i in range(count):self.lines.append(f'GEP215_WCS_ENTITY discovered={i+1} id={70+i} actualOrigin=<600,41,2600> launched=1 launchAge=0.01')
  times=[.05,.6,1.1,3.1,6.6,12.1,30.1,40.1,42.1]
  for n,t in enumerate(times,1):
   positions=[[0,0,0] if world_zero else [600+i,41,2600+(0 if stationary else min(t,1)*20)] for i in range(count)]
   row={'scope':SCOPE,'sequence':n,'elapsedSinceProgram':t,'ammo':after,'knownPayloadEntities':count,'entityIds':list(range(70,70+count)),'actualOrigins':positions,'travelObserved':[999]*count,'launchAges':[t]*count}
   self.write_row(n,row)
  if mode=='full':self.lines.append('GEP215_WCS_EMPTY requested=1 expectedNoAdditionalPayload=1')
  self.lines.append(f'GEP215_WCS_TERMINAL reason=diagnostic_complete_acceptance_pending ammo={after} rounds={count}')
  (self.root/'logs/console.log').write_text('\n'.join(self.lines))
  terminal={'scope':SCOPE,'reason':'diagnostic_complete_acceptance_pending','programRequested':True,'emptyProgramRequested':mode=='full','settledCrew':True,'ammo':after,'knownPayloadEntities':count,'screenshotsRequested':8}
  (self.profile/'GEP215-wcs-result.json').write_text(json.dumps(terminal))
  (self.root/'exit.json').write_text(json.dumps({'verifiedExited':True,'leaseReleased':True,'exitCode':0}))
 def write_row(self,n,row):(self.profile/f'GEP215-wcs-life-{n}.json').write_text(json.dumps(row))
 def result(self,mode='single'):return analyze(self.root,mode)
 def test_absent_evidence_never_passes(self):
  r=self.result();self.assertEqual(r['deterministicTelemetry']['status'],'NOT_TESTED');self.assertEqual(r['captureAvailability']['status'],'NOT_TESTED')
 def test_single_valid_telemetry_still_needs_pixels(self):
  self.fixture();r=self.result();self.assertEqual(r['deterministicTelemetry']['status'],'PASS');self.assertEqual(r['visibleCloud'],'NEEDS_PIXEL_REVIEW');self.assertEqual(r['overallFeatureReadiness'],'NEEDS_USER_TEST')
 def test_full_valid_with_empty_control(self):
  self.fixture('full');r=self.result('full');self.assertEqual(r['deterministicTelemetry']['status'],'PASS');self.assertEqual(r['lifecycle']['status'],'PASS')
 def test_reported_travel_cannot_spoof_actual_motion(self):
  self.fixture(stationary=True);r=self.result();self.assertEqual(r['deterministicTelemetry']['checks']['actualMotionForEveryPayload']['status'],'FAIL')
 def test_world_zero_fails(self):
  self.fixture(world_zero=True);r=self.result();self.assertEqual(r['deterministicTelemetry']['checks']['allObservedPositionsNearFixture']['status'],'FAIL')
 def test_duplicate_native_entity_discovery_fails(self):
  self.fixture();p=self.root/'logs/console.log';p.write_text(p.read_text()+'\n'+self.lines[3]);self.assertEqual(self.result()['deterministicTelemetry']['checks']['exactUniqueNativeLaunches']['status'],'FAIL')
 def test_unexpected_second_program_fails(self):
  self.fixture();p=self.root/'logs/console.log';p.write_text(p.read_text()+'\n'+self.lines[2]);self.assertEqual(self.result()['deterministicTelemetry']['checks']['oneAuthorizedProgramRequest']['status'],'FAIL')
 def test_extra_after_empty_fails(self):
  self.fixture('full');p=self.profile/'GEP215-wcs-life-6.json';row=json.loads(p.read_text());row['knownPayloadEntities']=9;p.write_text(json.dumps(row));self.assertEqual(self.result('full')['deterministicTelemetry']['checks']['emptyAddsNoObservedAmmoOrPayload']['status'],'FAIL')
 def test_missing_full_empty_control_fails(self):
  self.fixture('full');p=self.root/'logs/console.log';p.write_text('\n'.join(l for l in self.lines if 'GEP215_WCS_EMPTY' not in l));self.assertEqual(self.result('full')['deterministicTelemetry']['checks']['oneEmptyProgramAfterInitial']['status'],'FAIL')
 def test_mismatched_json_arrays_fail_schema(self):
  self.fixture();p=self.profile/'GEP215-wcs-life-3.json';row=json.loads(p.read_text());row['actualOrigins']=[];p.write_text(json.dumps(row));self.assertEqual(self.result()['deterministicTelemetry']['checks']['inputSchemaValid']['status'],'FAIL')
 def test_native_script_error_retained(self):
  self.fixture();p=self.root/'logs/console.log';p.write_text(p.read_text()+'\nSCRIPT (E): fixture exception');self.assertEqual(self.result()['deterministicTelemetry']['checks']['noNativeScriptOrFatalErrors']['status'],'FAIL')
 def test_raw_only_never_infers_movement(self):
  self.fixture();[p.unlink() for p in self.profile.glob('GEP215-wcs-life-*.json')];self.assertEqual(self.result()['deterministicTelemetry']['status'],'NOT_TESTED')
 def test_json_only_never_infers_callback(self):
  self.fixture();(self.root/'logs/console.log').unlink();self.assertEqual(self.result()['deterministicTelemetry']['status'],'NOT_TESTED')
 def test_headers_are_availability_only(self):
  self.fixture();header=b'BM'+struct.pack('<IHHI',58,0,0,54)+struct.pack('<IiiHHIIiiII',40,1,1,1,24,0,4,0,0,0,0)+b'\x00'*4
  for i in range(1,9):(self.profile/f'GEP215-wcs-frame-{i}.bmp').write_bytes(header)
  r=self.result();self.assertEqual(r['captureAvailability']['status'],'PASS');self.assertFalse(r['captureAvailability']['pixelContentReviewed']);self.assertEqual(r['visibleCloud'],'NEEDS_PIXEL_REVIEW')
 def test_malformed_terminal_fails(self):
  self.fixture();(self.profile/'GEP215-wcs-result.json').write_text('[]');self.assertEqual(self.result()['deterministicTelemetry']['checks']['inputSchemaValid']['status'],'FAIL')
 def test_negative_thresholds_rejected(self):
  with self.assertRaises(ValueError):analyze(self.root,'single',min_travel=-1)

if __name__=='__main__':unittest.main(verbosity=2)
