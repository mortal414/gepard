#ifdef WORKBENCH
// Original GEP033 diagnostic. No playback, registration, save, module/focus, process or lease control.
class GEP033_ResourceObservation : JsonApiStruct
{
 string label;
 string input;
 string canonical;
 string canonicalPath;
 bool hasGuid;
 bool pathLoadValid;
 bool canonicalLoadValid;
 string pathLoadedIdentity;
 string canonicalLoadedIdentity;

 void GEP033_ResourceObservation()
 {
  RegV("label"); RegV("input"); RegV("canonical"); RegV("canonicalPath");
  RegV("hasGuid"); RegV("pathLoadValid"); RegV("canonicalLoadValid");
  RegV("pathLoadedIdentity"); RegV("canonicalLoadedIdentity");
 }
}

class GEP033_GraphObservation : JsonApiStruct
{
 string label;
 bool containerAvailable;
 string containerClass;
 int bankCount = -1;
 ref array<string> events = {};
 ref array<string> mixerRoutes = {};
 ref array<string> mixerResources = {};

 void GEP033_GraphObservation()
 {
  RegV("label"); RegV("containerAvailable"); RegV("containerClass"); RegV("bankCount");
  RegV("events"); RegV("mixerRoutes"); RegV("mixerResources");
 }
}

class GEP033_ResolverReport : JsonApiStruct
{
 string status = "NATIVE_RESOLVER_OBSERVATIONS_ONLY";
 bool audibleImplementation = false;
 ref array<ref GEP033_ResourceObservation> resources = {};
 ref array<ref GEP033_GraphObservation> graphs = {};
 void GEP033_ResolverReport()
 {
  RegV("status"); RegV("audibleImplementation"); RegV("resources"); RegV("graphs");
 }
}

[WorkbenchPluginAttribute(name: "GEP033 Resolve stock audio (read only)", description: "Compare mounted paths and canonical resources; report limited stock/candidate graph metadata. No audio playback.", wbModules: { "ResourceManager" }, category: "Gepard QA")]
class GEP033_ResolveStockAudioPlugin : WorkbenchPlugin
{
 protected ref GEP033_ResolverReport m_Report;
 override void Run() { RunConfigured(); }
 override void RunCommandline() { RunConfigured(); }

 protected void RunConfigured()
 {
  ResourceManager manager = Workbench.GetModule(ResourceManager);
  string output;
  if (!manager || !manager.GetCmdLine("-gep033Output", output) || output.IsEmpty())
  {
   Print("GEP033_PROBE_CONFIG_ERROR missing -gep033Output; no observation run");
   return;
  }
  if (FileIO.FileExists(output))
  {
   Print("GEP033_PROBE_CONFIG_ERROR output already exists; preserve earlier evidence");
   return;
  }
  Probe(output);
 }

 protected string LoadedIdentity(Resource loaded)
 {
  if (!loaded || !loaded.IsValid()) return string.Empty;
  BaseResourceObject object = loaded.GetResource();
  if (!object) return string.Empty;
  return object.GetResourceName();
 }

 protected void GraphSummary(string label, Resource loaded)
 {
  GEP033_GraphObservation row = new GEP033_GraphObservation();
  row.label = label;
  BaseContainer graph;
  if (loaded && loaded.IsValid() && loaded.GetResource()) graph = loaded.GetResource().ToBaseContainer();
  row.containerAvailable = graph != null;
  if (graph)
  {
   row.containerClass = graph.GetClassName();
   if (graph.GetVarIndex("banks_local") >= 0)
   {
    BaseContainerList banks = graph.GetObjectArray("banks_local");
    if (banks) row.bankCount = banks.Count();
   }
   if (graph.GetVarIndex("sounds") >= 0)
   {
    BaseContainerList sounds = graph.GetObjectArray("sounds");
    if (sounds)
    {
     for (int i = 0; i < sounds.Count(); i++)
     {
      BaseContainer sound = sounds.Get(i);
      string eventName;
      if (sound && sound.Get("name", eventName)) row.events.Insert(eventName);
     }
    }
   }
   if (graph.GetVarIndex("mixers") >= 0)
   {
    BaseContainerList mixers = graph.GetObjectArray("mixers");
    if (mixers)
    {
     for (int j = 0; j < mixers.Count(); j++)
     {
      BaseContainer mixer = mixers.Get(j);
      if (!mixer) continue;
      string resource;
      if (mixer.GetVarIndex("res") >= 0 && mixer.Get("res", resource)) row.mixerResources.Insert(resource);
      if (mixer.GetVarIndex("inputs") >= 0)
      {
       array<string> routes = {};
       if (mixer.Get("inputs", routes))
       {
        foreach (string route : routes) row.mixerRoutes.Insert(route);
       }
      }
     }
    }
   }
  }
  row.Pack();
  m_Report.graphs.Insert(row);
  Print("GEP033_GRAPH_JSON " + row.AsString());
 }

 protected void Resolve(string label, string path, bool inspectGraph = false)
 {
  GEP033_ResourceObservation row = new GEP033_ResourceObservation();
  row.label = label;
  row.input = path;
  ResourceName canonical = Workbench.GetResourceName(path);
  row.canonical = canonical;
  row.canonicalPath = canonical.GetPath();
  row.hasGuid = canonical.Length() > 18 && canonical.StartsWith("{") && canonical.Substring(17, 1) == "}";
  // An absent optional graph and the negative control are reported without loading a missing file.
  if (label != "missing-control" && (label != "candidate-graph" || row.hasGuid))
  {
   Resource byPath = Resource.Load(path);
   Resource byCanonical = Resource.Load(canonical);
   row.pathLoadValid = byPath && byPath.IsValid();
   row.canonicalLoadValid = byCanonical && byCanonical.IsValid();
   row.pathLoadedIdentity = LoadedIdentity(byPath);
   row.canonicalLoadedIdentity = LoadedIdentity(byCanonical);
   if (inspectGraph) GraphSummary(label, byCanonical);
  }
  row.Pack();
  m_Report.resources.Insert(row);
  Print("GEP033_RESOURCE_JSON " + row.AsString());
 }

 protected void Probe(string output)
 {
  m_Report = new GEP033_ResolverReport();
  Print("GEP033_PROBE_BEGIN schema=1 source-only-tool; no playback/save/registration/session-control");
  Resolve("stock-very-slow", "Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_VerySlow.wav");
  Resolve("stock-slow", "Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Slow.wav");
  Resolve("stock-medium", "Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Medium.wav");
  Resolve("stock-fast", "Sounds/Vehicles/_SharedData/Tracks/Rotate/Vehicles_Tracks_Rotate_Fast.wav");
  Resolve("stock-wheel-left", "Sounds/Vehicles/_SharedData/Wheels/Vehicles_APC_Wheel_l_1.acp", true);
  Resolve("stock-wheel-right", "Sounds/Vehicles/_SharedData/Wheels/Vehicles_APC_Wheel_r_1.acp", true);
  Resolve("stock-engine", "Sounds/Vehicles/Wheeled/LAV25/Vehicles_LAV25_Engine.acp", true);
  Resolve("candidate-graph", "Sounds/Gepard/Tracks/GEP_TrackMovement.acp", true);
  Resolve("missing-control", "Sounds/Gepard/Tracks/GEP033_KnownMissing_Control.wav");
  m_Report.Pack();
  bool saved = m_Report.SaveToFile(output);
  if (saved) Print("GEP033_PROBE_END observations=9 saved=1; resource loading is not graph compile/event/playback acceptance");
  else Print("GEP033_PROBE_END observations=9 saved=0; failed to save report; retain console and diagnose");
 }
}
#endif
