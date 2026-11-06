"""Soch v2 ni import qiladi va M_Soch_PastTugun ni qayta quradi (masked, tola chiziqlari, notekis chet).
UV0.x - o'rtadagi farqdan taraladigan tola yo'nalishi; UV0.y (FBX'da teskari) - chetdan masofa."""
import unreal, math
from pathlib import Path
root = Path(unreal.Paths.project_dir())
ats = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
dest = "/Game/Bozor/Romol/HairFit"
name = "SK_Soch_PastTugun"
skeleton = unreal.load_asset("/Game/Bozor/Romol/SK_Romol_V2").skeleton
task = unreal.AssetImportTask()
task.filename = str(root / "Assets/Romol/HairFit" / (name + ".fbx"))
task.destination_path = dest; task.destination_name = name
task.automated = True; task.save = True; task.replace_existing = True
opt = unreal.FbxImportUI()
opt.import_mesh = True; opt.import_as_skeletal = True
opt.mesh_type_to_import = unreal.FBXImportType.FBXIT_SKELETAL_MESH
opt.automated_import_should_detect_type = False
opt.skeleton = skeleton
opt.import_materials = False; opt.import_textures = False; opt.import_animations = False; opt.create_physics_asset = False
opt.skeletal_mesh_import_data.set_editor_property("update_skeleton_reference_pose", False)
opt.skeletal_mesh_import_data.set_editor_property("use_t0_as_ref_pose", False)
task.options = opt; task.factory = unreal.FbxFactory()
ats.import_asset_tasks([task])
mesh = unreal.load_asset(dest + "/" + name)
assert mesh.skeleton == skeleton

hairmat = unreal.load_asset(dest + "/M_Soch_PastTugun")
mel.delete_all_material_expressions(hairmat)
hairmat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_MASKED)
hairmat.set_editor_property("opacity_mask_clip_value", 0.5)
hairmat.set_editor_property("two_sided", True)
X = [0]
def E(cls, **props):
    e = mel.create_material_expression(hairmat, cls, -900 + X[0] * 10, X[0] * 40); X[0] += 1
    for k, v in props.items(): e.set_editor_property(k, v)
    return e
def L(a, b, pin=""): mel.connect_material_expressions(a, "", b, pin)
uv = E(unreal.MaterialExpressionTextureCoordinate)
u = E(unreal.MaterialExpressionComponentMask, r=True, g=False, b=False, a=False); L(uv, u)
vv = E(unreal.MaterialExpressionComponentMask, r=False, g=True, b=False, a=False); L(uv, vv)
edge = E(unreal.MaterialExpressionOneMinus); L(vv, edge)                  # 0 chetda, 1 ichkarida
def wave(freq, phase):
    m = E(unreal.MaterialExpressionMultiply, const_b=freq); L(u, m, "A")
    a = E(unreal.MaterialExpressionAdd, const_b=phase); L(m, a, "A")
    s = E(unreal.MaterialExpressionSine, period=2 * math.pi); L(a, s)
    h = E(unreal.MaterialExpressionMultiply, const_b=.5); L(s, h, "A")
    o = E(unreal.MaterialExpressionAdd, const_b=.5); L(h, o, "A")
    return o
stripe = wave(2 * math.pi * 600, 0.0)
clump = wave(2 * math.pi * 90, 1.3)
s7 = E(unreal.MaterialExpressionMultiply, const_b=.7); L(stripe, s7, "A")
c3 = E(unreal.MaterialExpressionMultiply, const_b=.3); L(clump, c3, "A")
mix = E(unreal.MaterialExpressionAdd); L(s7, mix, "A"); L(c3, mix, "B")
dark = E(unreal.MaterialExpressionConstant3Vector, constant=unreal.LinearColor(.006, .0035, .0022, 1))
light = E(unreal.MaterialExpressionConstant3Vector, constant=unreal.LinearColor(.03, .018, .011, 1))
col = E(unreal.MaterialExpressionLinearInterpolate); L(dark, col, "A"); L(light, col, "B"); L(mix, col, "Alpha")
mel.connect_material_property(col, "", unreal.MaterialProperty.MP_BASE_COLOR)
rough = E(unreal.MaterialExpressionConstant, r=.5); mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
spec = E(unreal.MaterialExpressionConstant, r=.35); mel.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)
# niqob: edge*2.2 + stripe*0.6 + clump*0.3 - 0.35  (>0.5 ko'rinadi) - chetda tolalar uchi notekis
e2 = E(unreal.MaterialExpressionMultiply, const_b=2.2); L(edge, e2, "A")
s6 = E(unreal.MaterialExpressionMultiply, const_b=.6); L(stripe, s6, "A")
a1 = E(unreal.MaterialExpressionAdd); L(e2, a1, "A"); L(s6, a1, "B")
a2 = E(unreal.MaterialExpressionAdd); L(a1, a2, "A"); L(c3, a2, "B")
a3 = E(unreal.MaterialExpressionAdd, const_b=-.35); L(a2, a3, "A")
mel.connect_material_property(a3, "", unreal.MaterialProperty.MP_OPACITY_MASK)
mel.layout_material_expressions(hairmat)
mel.recompile_material(hairmat)
unreal.EditorAssetLibrary.save_loaded_asset(hairmat)
slots = mesh.materials; slots[0].material_interface = hairmat; mesh.materials = slots
unreal.EditorAssetLibrary.save_loaded_asset(mesh)
print("HAIR V2 IMPORTED", mesh.get_bounds().box_extent)
