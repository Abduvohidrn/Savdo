import bpy, bmesh, math, json
from pathlib import Path
from mathutils import Vector
from mathutils.bvhtree import BVHTree
P=Path(r"D:/Unreal Projects/BozorUnreal")
OUT=P/"Assets/Romol/HairFit"
OUT.mkdir(exist_ok=True)
bpy.ops.wm.open_mainfile(filepath=str(P/"Saved/Romol/Romol.blend"))
sc=bpy.context.scene
sc.render.engine="CYCLES"
sc.cycles.samples=24
head=bpy.data.objects["Bosh"]
rom=bpy.data.objects["Romol"]
arm=bpy.data.objects["ARM"]
C=Vector((0,.7,163.8))
bvh=BVHTree.FromPolygons([v.co for v in head.data.vertices],[tuple(p.vertices) for p in head.data.polygons if p.material_index==3])
vs=[]; fs=[]; uvs=[]
N=160; R=25
for j in range(R):
 t=j/R
 for i in range(N):
  a=2*math.pi*i/N
  rear=(1-math.cos(a))/2
  edge=math.radians(66+64*rear**1.7-13*math.exp(-((math.acos(math.cos(a))-math.radians(85))/math.radians(22))**2))
  edge+=math.radians(1.5*math.sin(3*a)+.32*math.sin(19*a)+.12*math.sin(47*a))*(1-rear)
  theta=edge*(1-t)
  d=Vector((math.sin(theta)*math.sin(a),-math.sin(theta)*math.cos(a),math.cos(theta)))
  hit,n,_,_=bvh.ray_cast(C+d*28,-d,28)
  assert hit is not None
  # Small ridges follow the sweep toward the nape.
  ridge=.035*(1+math.sin(64*a+12*t))*math.sin(theta)
  vs.append(hit+d*(.28+ridge+.08*math.sin(theta)))
for j in range(R-1):
 for i in range(N):
  fs.append((j*N+i,j*N+(i+1)%N,(j+1)*N+(i+1)%N,(j+1)*N+i))
  uvs.append(((i/N,j/R),((i+1)/N,j/R),((i+1)/N,(j+1)/R),(i/N,(j+1)/R)))
pole=len(vs)
vs.append(C+Vector((0,0,12.35)))
for i in range(N):
 fs.append(((R-1)*N+i,(R-1)*N+(i+1)%N,pole))
 uvs.append(((i/N,(R-1)/R),((i+1)/N,(R-1)/R),((i+.5)/N,1)))
me=bpy.data.meshes.new("Soch_PastTugun")
me.from_pydata(vs,[],fs)
uv=me.uv_layers.new()
for f,coords in zip(me.polygons,uvs):
 f.use_smooth=True
 for li,co in zip(f.loop_indices,coords): uv.data[li].uv=co
hair=bpy.data.objects.new("Soch_PastTugun",me)
sc.collection.objects.link(hair)
# A compact twisted coil at the nape, partly tucked inside the kerchief.
bpy.ops.mesh.primitive_uv_sphere_add(segments=48,ring_count=24,location=(0,10.8,155.3))
bun=bpy.context.object
bun.name="Low_Bun"
for v in bun.data.vertices:
 p=v.co.copy()
 a=math.atan2(p.z,p.x)
 ridge=1+.035*math.sin(13*a+6*p.y)
 v.co=Vector((p.x*3.8*ridge,p.y*2.2*ridge,p.z*2.65*ridge))
for f in bun.data.polygons: f.use_smooth=True
for o in bpy.context.selected_objects:o.select_set(False)
hair.select_set(True);bun.select_set(True)
bpy.context.view_layer.objects.active=hair
bpy.ops.object.join()
bm=bmesh.new();bm.from_mesh(hair.data)
bmesh.ops.recalc_face_normals(bm,faces=bm.faces)
bm.to_mesh(hair.data);bm.free()
mat=bpy.data.materials.new("M_Soch_PastTugun")
bs=mat.node_tree.nodes.get("Principled BSDF")
bs.inputs["Base Color"].default_value=(.018,.010,.007,1)
bs.inputs["Roughness"].default_value=.55
bs.inputs["Specular IOR Level"].default_value=.28
mat.diffuse_color=(.018,.010,.007,1)
hair.data.materials.append(mat)
hair.vertex_groups.new(name="head").add(list(range(len(hair.data.vertices))),1,"REPLACE")
hair.parent=arm
hair.matrix_parent_inverse=arm.matrix_world.inverted()
hair.modifiers.new("Citizen head","ARMATURE").object=arm

# Reserve volume over the low bun, and lift the front hem to reveal the hairline.
for v in rom.data.vertices:
 p=v.co
 front=math.exp(-((p.y+8)/4.5)**2)*math.exp(-((p.z-171)/5)**2)
 p.z+=.45*front
 p.y+=.4*front
 back=math.exp(-(p.x/5.3)**2-((p.z-156)/4.2)**2)*max(0,min(1,(p.y-5)/4))
 p.y+=2.15*back
# Keep the small tied ends outside the bun.
for v in rom.data.vertices:
 if v.co.z<157 and v.co.y>11:
  v.co.y+=.6*math.exp(-((v.co.z-154)/7)**2)
sc.unit_settings.scale_length=.01
for obj,name in ((hair,"SK_Soch_PastTugun"),(rom,"SK_Romol_HairFit")):
 for o in bpy.context.selected_objects:o.select_set(False)
 obj.select_set(True);arm.select_set(True)
 bpy.context.view_layer.objects.active=arm
 arm.name="root"
 bpy.ops.export_scene.fbx(filepath=str(OUT/(name+".fbx")),use_selection=True,object_types={"ARMATURE","MESH"},apply_unit_scale=True,apply_scale_options="FBX_SCALE_NONE",add_leaf_bones=False,bake_anim=False,mesh_smooth_type="FACE",path_mode="STRIP")
 arm.name="ARM"
# Review with the blue variant.
image=bpy.data.images.load(str(P/"Assets/Romol/Variants/T_Romol_01_Kok_Moychechak.png"))
image.pack()
for n in rom.data.materials[0].node_tree.nodes:
 if n.type=="TEX_IMAGE":n.image=image
camera=sc.camera
target=Vector((0,1,162))
sc.render.resolution_x=sc.render.resolution_y=700
for name,d in (("Front",Vector((.45,-1,.1))),("Back",Vector((.5,1,.1)))):
 camera.location=target+d.normalized()*75
 camera.rotation_euler=(target-camera.location).to_track_quat("-Z","Y").to_euler()
 sc.render.filepath=str(OUT/("HairFit_"+name+".png"))
 bpy.ops.render.render(write_still=True)
bpy.context.preferences.filepaths.save_version=0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/"Romol_Soch.blend"))
print("SAVED HAIR AND SCARF",len(hair.data.vertices))

