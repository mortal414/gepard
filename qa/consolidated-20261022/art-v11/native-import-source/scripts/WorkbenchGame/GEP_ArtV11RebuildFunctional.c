[WorkbenchPluginAttribute(name:"Rebuild five private V11 articulated models",category:"Gepard private art",wbModules:{"ResourceManager"})]
class GEP_ArtV11RebuildFunctional : ResourceManagerPlugin {
 override void RunCommandline() { ResourceManager manager=Workbench.GetModule(ResourceManager);if(!manager)return;
  array<string> files={"D:/ArmaProjects/Gepard-Consolidated-20261022/native-stage/gepard/Assets/ArtV11/Mobility/MobilityRig.fbx","D:/ArmaProjects/Gepard-Consolidated-20261022/native-stage/gepard/Assets/ArtV11/Gepard/GepardTurretRig.fbx","D:/ArmaProjects/Gepard-Consolidated-20261022/native-stage/gepard/Assets/ArtV11/Gepard/RemainingExterior669/RightStorage669.fbx","D:/ArmaProjects/Gepard-Consolidated-20261022/native-stage/gepard/Assets/ArtV11/Gepard/GepardGunLeft.fbx","D:/ArmaProjects/Gepard-Consolidated-20261022/native-stage/gepard/Assets/ArtV11/Gepard/GepardGunRight.fbx"};
  manager.RebuildResourceFiles(files,"PC");Print("GEP_ARTV11_FUNCTIONAL_5MODEL_BATCH_QUEUED");
 }
}
