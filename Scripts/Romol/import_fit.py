"""Import fitted scarf/hair and create muted cloth materials."""
import unreal,json
from pathlib import Path
root=Path(unreal.Paths.project_dir())
ats=unreal.AssetToolsHelpers.get_asset_tools()
mel=unreal.MaterialEditingLibrary
dest="/Game/Bozor/Romol/HairFit"
skeleton=unreal.load_asset("/Game/Bozor/Romol/SK_Romol_V2").skeleton
for name in ("SK_Soch_PastTugun","SK_Romol_HairFit"):
 task=unreal.AssetImportTask()
 task.filename=str(root/"Assets/Romol/HairFit"/(name+".fbx"))
 task.destination_path=dest
 task.destination_name=name
 task.automated=True
 task.save=True
 task.replace_existing=True
 opt=unreal.FbxImportUI()
 opt.import_mesh=True
 opt.import_as_skeletal=True
 opt.mesh_type_to_import=unreal.FBXImportType.FBXIT_SKELETAL_MESH
 opt.automated_import_should_detect_type=False
 opt.skeleton=skeleton
 opt.import_materials=False
 opt.import_textures=False
 opt.import_animations=False
 opt.create_physics_asset=False
 opt.skeletal_mesh_import_data.set_editor_property("update_skeleton_reference_pose",False)
 opt.skeletal_mesh_import_data.set_editor_property("use_t0_as_ref_pose",False)
 task.options=opt
 task.factory=unreal.FbxFactory()
 ats.import_asset_tasks([task])
 assert unreal.load_asset(dest+"/"+name).skeleton==skeleton

parent_path=dest+"/M_Romol_Muted"
parent=unreal.load_asset(parent_path) if unreal.EditorAssetLibrary.does_asset_exist(parent_path) else unreal.EditorAssetLibrary.duplicate_asset("/Game/Bozor/Romol/M_Romol",parent_path)
if "Tone" not in [str(n) for n in mel.get_scalar_parameter_names(parent)]:
 source=mel.get_material_property_input_node(parent,unreal.MaterialProperty.MP_BASE_COLOR)
 desat=mel.create_material_expression(parent,unreal.MaterialExpressionDesaturation,-250,0)
 amount=mel.create_material_expression(parent,unreal.MaterialExpressionScalarParameter,-450,150)
 amount.set_editor_property("parameter_name","Desaturation")
 amount.set_editor_property("default_value",.28)
 tone=mel.create_material_expression(parent,unreal.MaterialExpressionScalarParameter,-220,240)
 tone.set_editor_property("parameter_name","Tone")
 tone.set_editor_property("default_value",.5)
 mult=mel.create_material_expression(parent,unreal.MaterialExpressionMultiply,0,0)
 mel.connect_material_expressions(source,"RGB",desat,"")
 mel.connect_material_expressions(amount,"",desat,"Fraction")
 mel.connect_material_expressions(desat,"",mult,"A")
 mel.connect_material_expressions(tone,"",mult,"B")
 mel.connect_material_property(mult,"",unreal.MaterialProperty.MP_BASE_COLOR)
 spec=mel.create_material_expression(parent,unreal.MaterialExpressionConstant,0,200)
 spec.set_editor_property("r",.2)
 mel.connect_material_property(spec,"",unreal.MaterialProperty.MP_SPECULAR)
 mel.recompile_material(parent)
unreal.EditorAssetLibrary.save_loaded_asset(parent)

items=json.loads((root/"Assets/Romol/Variants/variants.json").read_text())
for item in items:
 name="MI_Romol_"+item["name"]+"_Muted"
 path=dest+"/"+name
 mi=unreal.load_asset(path) if unreal.EditorAssetLibrary.does_asset_exist(path) else ats.create_asset(name,dest,unreal.MaterialInstanceConstant,unreal.MaterialInstanceConstantFactoryNew())
 mel.set_material_instance_parent(mi,parent)
 tex=unreal.load_asset("/Game/Bozor/Romol/Variants/Tex/"+Path(item["base_color"]).stem)
 mel.set_material_instance_texture_parameter_value(mi,"Naqsh",tex)
 mel.set_material_instance_scalar_parameter_value(mi,"Tone",.46)
 mel.set_material_instance_scalar_parameter_value(mi,"Desaturation",.28)
 mel.set_material_instance_scalar_parameter_value(mi,"Gadir",.84)
 mel.update_material_instance(mi)
 assert mel.get_material_instance_texture_parameter_value(mi,"Naqsh")==tex
 unreal.EditorAssetLibrary.save_loaded_asset(mi)

hair_path=dest+"/M_Soch_PastTugun"
hairmat=unreal.load_asset(hair_path) if unreal.EditorAssetLibrary.does_asset_exist(hair_path) else ats.create_asset("M_Soch_PastTugun",dest,unreal.Material,unreal.MaterialFactoryNew())
mel.delete_all_material_expressions(hairmat)
def expr(cls,x,y):return mel.create_material_expression(hairmat,cls,x,y)
uv=expr(unreal.MaterialExpressionTextureCoordinate,-700,0)
mask=expr(unreal.MaterialExpressionComponentMask,-500,0)
mask.set_editor_property("r",True); mask.set_editor_property("g",False)
freq=expr(unreal.MaterialExpressionMultiply,-300,0)
freq.set_editor_property("const_b",160)
sine=expr(unreal.MaterialExpressionSine,-100,0)
lerp=expr(unreal.MaterialExpressionLinearInterpolate,100,0)
dark=expr(unreal.MaterialExpressionConstant3Vector,-100,150)
dark.set_editor_property("constant",unreal.LinearColor(.009,.005,.003,1))
light=expr(unreal.MaterialExpressionConstant3Vector,-100,250)
light.set_editor_property("constant",unreal.LinearColor(.028,.016,.009,1))
mel.connect_material_expressions(uv,"",mask,"")
mel.connect_material_expressions(mask,"",freq,"A")
mel.connect_material_expressions(freq,"",sine,"")
mel.connect_material_expressions(sine,"",lerp,"Alpha")
mel.connect_material_expressions(dark,"",lerp,"A")
mel.connect_material_expressions(light,"",lerp,"B")
mel.connect_material_property(lerp,"",unreal.MaterialProperty.MP_BASE_COLOR)
rough=expr(unreal.MaterialExpressionConstant,100,200);rough.set_editor_property("r",.57)
mel.connect_material_property(rough,"",unreal.MaterialProperty.MP_ROUGHNESS)
spec=expr(unreal.MaterialExpressionConstant,100,300);spec.set_editor_property("r",.25)
mel.connect_material_property(spec,"",unreal.MaterialProperty.MP_SPECULAR)
mel.recompile_material(hairmat)
unreal.EditorAssetLibrary.save_loaded_asset(hairmat)
for name,mat in (("SK_Soch_PastTugun",hairmat),("SK_Romol_HairFit",unreal.load_asset(dest+"/MI_Romol_01_Kok_Moychechak_Muted"))):
 mesh=unreal.load_asset(dest+"/"+name)
 slots=mesh.materials
 slots[0].material_interface=mat
 mesh.materials=slots
 unreal.EditorAssetLibrary.save_loaded_asset(mesh)
print("IMPORTED two skeletal meshes, six muted variants, hair material")



