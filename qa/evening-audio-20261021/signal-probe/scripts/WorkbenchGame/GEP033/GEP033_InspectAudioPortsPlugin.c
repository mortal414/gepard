#ifdef WORKBENCH
// Fresh owned QA Workbench session only: changes its opened resource, never saves or previews.
// Parent controls native launch/closure. Do not run against a human-owned editor.
[WorkbenchPluginAttribute(name: "GEP033 Inspect stock graph ports (read only)", description: "Load stock audio graphs in an owned AudioEditor module and report bounded nonpayload port/config metadata; no save or playback.", wbModules: { "AudioEditor" }, category: "Gepard QA")]
class GEP033_InspectAudioPortsPlugin : GEP033_ResolveStockAudioPlugin
{
 override protected void GraphSummary(string label, ResourceName canonical, Resource loaded)
 {
  GEP033_GraphObservation row = new GEP033_GraphObservation();
  row.label = label;
  row.containerLoadRoute = "AudioEditor.SetOpenedResource/GetContainer";
  AudioEditor editor = Workbench.GetModule(AudioEditor);
  BaseContainer graph;
  if (editor && editor.SetOpenedResource(canonical))
  {
   int count = editor.GetNumContainers();
   for (int i = 0; i < count; i++)
   {
    BaseContainer candidate = editor.GetContainer(i);
    if (candidate && candidate.GetClassName() == "AudioClass")
    {
     graph = candidate;
     break;
    }
   }
  }
  // No guessed fallback container, Save, ExecuteAction, preview, or recursive resource export.
  DescribeGraph(row, graph);
 }
}
#endif
