// PRIVATE QA observer/capture guard; parent owns subject, camera, world and all native lifecycle.
class GEP_ArtNativePartRow20261022 : JsonApiStruct
{
 string skin; string path; string name; string prefab; string visual;
 int modelIndex = -1; int materialCount; int boneCount; int configMaterialAssignments = -1;
 bool parentMatches; bool truncated;
 ref array<string> materials = {}; ref array<string> transform = {};
 vector boundsMin; vector boundsMax;
 void GEP_ArtNativePartRow20261022()
 {
  RegV("skin"); RegV("path"); RegV("name"); RegV("prefab"); RegV("visual");
  RegV("modelIndex"); RegV("materialCount"); RegV("boneCount"); RegV("configMaterialAssignments"); RegV("parentMatches");
  RegV("truncated"); RegV("materials"); RegV("transform"); RegV("boundsMin"); RegV("boundsMax");
 }
}
class GEP_ArtNativeBoneRow20261022 : JsonApiStruct
{
 string skin; string ownerPath; string name; int index;
 bool matrixRead; bool localMatrixRead;
 ref array<string> matrixRaw = {}; ref array<string> localMatrixRaw = {};
 void GEP_ArtNativeBoneRow20261022()
 {
  RegV("skin"); RegV("ownerPath"); RegV("name"); RegV("index");
  RegV("matrixRead"); RegV("localMatrixRead"); RegV("matrixRaw"); RegV("localMatrixRaw");
 }
}
class GEP_ArtNativeMaterialRow20261022 : JsonApiStruct
{
 string skin; string ownerPath; string sourceRole; string assignedResource;
 bool resourceLoadValid;
 string scope = "RESOLVED_PREFAB_CONFIG_NOT_GPU_BINDING_PROOF";
 void GEP_ArtNativeMaterialRow20261022()
 {
  RegV("skin"); RegV("ownerPath"); RegV("sourceRole"); RegV("assignedResource");
  RegV("resourceLoadValid"); RegV("scope");
 }
}
class GEP_ArtNativeQualification20261022
{
 protected ref array<ResourceName> m_Models = {
  "{578CF9C31193CC94}Assets/ArtApproved20261022/Mobility/MobilityRig.xob",
  "{4D7043CCA2E8BD79}Assets/ArtApproved20261022/Gepard/GepardTurretRig.xob",
  "{C6D9B89642E424E3}Assets/ArtV11/Gepard/RemainingExterior669/RightStorage669.xob",
  "{2A7F69D8A7902DE2}Assets/ArtApproved20261022/Gepard/GepardGunLeft.xob",
  "{3772144B4AA90B1E}Assets/ArtApproved20261022/Gepard/GepardGunRight.xob",
  "{B621A5631346B7A3}Assets/ArtApproved20261022/Gepard/ReferenceFittings668/CoverLeft668.xob",
  "{418DE4B4FC8541A0}Assets/ArtApproved20261022/Gepard/ReferenceFittings668/CoverRight668.xob",
  "{C23AF0E6361BFB2C}Assets/ArtApproved20261022/Gepard/RearCover671/RightShieldCover671.xob"
 };
 protected ref array<int> m_Seen = {};
 protected int m_Parts; protected bool m_Truncated; protected bool m_ReadFailure;
 protected IEntity m_Camo; protected IEntity m_Charcoal;
 protected IEntity m_Subject; protected IEntity m_CameraActor;
 protected string m_Skin; protected string m_View; protected float m_CaptureStartMs;
 protected bool m_Staged;
 protected ref array<string> m_CapturedKeys = {};

 protected ResourceName ExpectedPrefab(string skin)
 {
  if (skin == "Camo") return "{2150FC6F1E081188}Prefabs/ArtApproved20261022/Camo/GEP_MobileCarrier.et";
  if (skin == "Charcoal") return "{8095474EC9806D0D}Prefabs/ArtApproved20261022/Charcoal/GEP_MobileCarrier.et";
  return string.Empty;
 }
 protected string PrefabName(IEntity entity)
 {
  EntityPrefabData data = entity.GetPrefabData();
  if (!data) return string.Empty;
  return data.GetPrefabName();
 }
 protected int ReadMaterials(IEntity entity, string skin, string ownerPath)
 {
  EntityPrefabData data = entity.GetPrefabData();
  if (!data) return -1;
  BaseContainer prefab = data.GetPrefab();
  if (!prefab || prefab.GetVarIndex("components") < 0) return -1;
  BaseContainerList components = prefab.GetObjectArray("components");
  if (!components) return -1;
  int readCount;
  int count = components.Count();
  if (count > 128) { count = 128; m_Truncated = true; }
  for (int i = 0; i < count; i++)
  {
   BaseContainer component = components.Get(i);
   if (!component || component.GetClassName() != "MeshObject" || component.GetVarIndex("Materials") < 0) continue;
   BaseContainerList assignments = component.GetObjectArray("Materials");
   if (!assignments) continue;
   int materialCount = assignments.Count();
   if (materialCount > 256) { materialCount = 256; m_Truncated = true; }
   for (int j = 0; j < materialCount; j++)
   {
    BaseContainer assignment = assignments.Get(j);
    if (!assignment) continue;
    GEP_ArtNativeMaterialRow20261022 row = new GEP_ArtNativeMaterialRow20261022();
    row.skin = skin; row.ownerPath = ownerPath;
    assignment.Get("SourceMaterial", row.sourceRole);
    assignment.Get("AssignedMaterial", row.assignedResource);
    if (!row.assignedResource.IsEmpty())
    {
     Resource material = Resource.Load(row.assignedResource);
     row.resourceLoadValid = material && material.IsValid();
    }
    if (!row.resourceLoadValid) m_ReadFailure = true;
    readCount++;
    row.Pack(); Print("GEP_ART_NATIVE_MATERIAL " + row.AsString());
   }
  }
  return readCount;
 }
 protected void Part(IEntity entity, IEntity expectedParent, string skin, string path, int depth)
 {
  if (!entity) return;
  if (depth > 32 || m_Parts >= 256) { m_Truncated = true; return; }
  m_Parts++;
  GEP_ArtNativePartRow20261022 row = new GEP_ArtNativePartRow20261022();
  row.skin = skin; row.path = path; row.name = entity.GetName(); row.prefab = PrefabName(entity);
  row.parentMatches = !expectedParent || entity.GetParent() == expectedParent;
  if (!row.parentMatches) m_ReadFailure = true;
  vector transform[4]; entity.GetWorldTransform(transform);
  for (int t = 0; t < 4; t++) row.transform.Insert(transform[t].ToString());
  entity.GetBounds(row.boundsMin, row.boundsMax);
  VObject visual = entity.GetVObject();
  if (visual)
  {
   row.visual = visual.GetResourceName();
   for (int i = 0; i < m_Models.Count(); i++)
   {
    if (row.visual == m_Models[i]) { row.modelIndex = i; m_Seen[i] = m_Seen[i] + 1; }
   }
   string materials[512]; row.materialCount = visual.GetMaterials(materials);
   if (row.materialCount > 512) { row.truncated = true; m_Truncated = true; }
   for (int m = 0; m < row.materialCount && m < 512; m++) row.materials.Insert(materials[m]);
  }
  if (row.modelIndex >= 0)
  {
   array<string> names = {}; entity.GetBoneNames(names); row.boneCount = names.Count();
   if (row.modelIndex <= 2 && names.IsEmpty()) m_ReadFailure = true;
   int count = names.Count();
   if (count > 512) { count = 512; row.truncated = true; m_Truncated = true; }
   for (int b = 0; b < count; b++)
   {
    GEP_ArtNativeBoneRow20261022 bone = new GEP_ArtNativeBoneRow20261022();
    bone.skin = skin; bone.ownerPath = path; bone.name = names[b]; bone.index = entity.GetBoneIndex(names[b]);
    vector matrix[4]; vector localMatrix[4];
    // TNodeId is a signed name hash, not a nonnegative ordinal.
    bone.matrixRead = entity.GetBoneMatrix(bone.index, matrix);
    bone.localMatrixRead = entity.GetBoneLocalMatrix(bone.index, localMatrix);
    if (!bone.matrixRead || !bone.localMatrixRead) m_ReadFailure = true;
    for (int k = 0; k < 4; k++)
    {
     if (bone.matrixRead) bone.matrixRaw.Insert(matrix[k].ToString());
     if (bone.localMatrixRead) bone.localMatrixRaw.Insert(localMatrix[k].ToString());
    }
    bone.Pack(); Print("GEP_ART_NATIVE_BONE " + bone.AsString());
   }
   row.configMaterialAssignments = ReadMaterials(entity, skin, path);
   if (skin == "Charcoal" && row.modelIndex == 0 && row.configMaterialAssignments <= 0) m_ReadFailure = true;
  }
  row.Pack(); Print("GEP_ART_NATIVE_PART " + row.AsString());
  IEntity child = entity.GetChildren(); int childIndex;
  while (child)
  {
   if (childIndex >= 256) { m_Truncated = true; break; }
   Part(child, entity, skin, path + "/" + childIndex.ToString(), depth + 1);
   child = child.GetSibling(); childIndex++;
  }
 }
 bool Inspect(IEntity root, string skin)
 {
  if (skin == "Camo") m_Camo = null;
  if (skin == "Charcoal") m_Charcoal = null;
  if (!root || ExpectedPrefab(skin).IsEmpty() || PrefabName(root) != ExpectedPrefab(skin))
  { Print("GEP_ART_NATIVE_NOT_TESTED wrong-or-unbound-functional-prefab"); return false; }
  if (!root.FindComponent(VehicleWheeledSimulation))
  { Print("GEP_ART_NATIVE_NOT_TESTED functional-wheeled-component-missing"); return false; }
  m_Seen.Clear(); for (int i = 0; i < m_Models.Count(); i++) m_Seen.Insert(0);
  m_Parts = 0; m_Truncated = false; m_ReadFailure = false;
  Print("GEP_ART_NATIVE_BEGIN skin=" + skin + " observations-only dynamic=NOT_TESTED");
  Part(root, null, skin, "root", 0);
  bool complete = !m_Truncated && !m_ReadFailure;
  for (int j = 0; j < m_Models.Count(); j++)
  {
   PrintFormat("GEP_ART_NATIVE_MODEL skin=%1 modelIndex=%2 count=%3 expected=%4", skin, j, m_Seen[j], m_Models[j]);
   if (m_Seen[j] != 1) complete = false;
  }
  PrintFormat("GEP_ART_NATIVE_END skin=%1 parts=%2 complete=%3 skeletonParents=NOT_TESTED effectiveGpuMaterials=NOT_TESTED skinning=NOT_TESTED", skin, m_Parts, complete);
  if (complete && skin == "Camo") m_Camo = root;
  if (complete && skin == "Charcoal") m_Charcoal = root;
  return complete;
 }
 bool BeginView(IEntity subject, IEntity alreadyOwnedCameraActor, string skin, string view)
 {
  if (m_Staged) return false;
  if (!subject || !alreadyOwnedCameraActor || (skin == "Camo" && subject != m_Camo) || (skin == "Charcoal" && subject != m_Charcoal)) return false;
  if (skin != "Camo" && skin != "Charcoal") return false;
  array<string> views = {"front3q", "rear3q", "left", "right", "guns", "reargrille", "rearfar"};
  if (!views.Contains(view) || m_CapturedKeys.Contains(skin + "-" + view)) return false;
  m_Subject = subject; m_CameraActor = alreadyOwnedCameraActor; m_Skin = skin; m_View = view;
  m_CaptureStartMs = GetGame().GetWorld().GetWorldTime(); m_Staged = true;
  PrintFormat("GEP_ART_NATIVE_VIEW_BEGIN skin=%1 view=%2 worldMs=%3 cameraPlacementOwnedByCaller=1", skin, view, m_CaptureStartMs);
  return true;
 }
 bool TryCapture()
 {
  if (!m_Staged || !m_Subject || !m_CameraActor || !GetGame().InPlayMode()) return false;
  if (GetGame().GetWorld().GetTimeScale() != 1) return false;
  if (GetGame().GetWorld().GetWorldTime() - m_CaptureStartMs < 4000) return false;
  PlayerController player = GetGame().GetPlayerController();
  if (!player || player.GetControlledEntity() != m_CameraActor || !player.GetPlayerCamera()) return false;
  if (GetGame().GetCameraManager().CurrentCamera() != player.GetPlayerCamera()) return false;
  if (ArmaReforgerLoadingAnim.IsOpen() || GetGame().GetMenuManager().IsAnyMenuOpen()) return false;
  vector camera[4]; player.GetPlayerCamera().GetWorldTransform(camera);
  PrintFormat("GEP_ART_NATIVE_VIEW_READBACK skin=%1 view=%2 worldMs=%3 timeScale=%4 subjectPosition=%5 camera0=%6 camera1=%7 camera2=%8 cameraEye=%9", m_Skin, m_View, GetGame().GetWorld().GetWorldTime(), GetGame().GetWorld().GetTimeScale(), m_Subject.GetOrigin(), camera[0], camera[1], camera[2], camera[3]);
  string key = m_Skin + "-" + m_View; m_CapturedKeys.Insert(key); m_Staged = false;
  bool requested = System.MakeScreenshot("$profile:artnative-demo10-new-" + key);
  PrintFormat("GEP_ART_NATIVE_CAPTURE skin=%1 view=%2 requested=%3 nativePixelsValidation=PENDING humanAcceptance=PENDING", m_Skin, m_View, requested);
  return requested;
 }
}
