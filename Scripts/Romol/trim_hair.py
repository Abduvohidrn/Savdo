"""Ro'mol ostida qoladigan soch tepasini kesadi: faqat soch chizig'i lentasi va ensadagi tugun qoladi.
Unreal'da tepadagi soch qobig'i ro'moldan teshib chiqardi (oradagi bo'shliq ~0.3 sm edi).
blender -b --factory-startup --python trim_hair.py
"""
import bpy, bmesh
from pathlib import Path
from mathutils import Vector
from mathutils.bvhtree import BVHTree
P = Path(r"D:/Unreal Projects/BozorUnreal")
OUT = P / "Assets/Romol/HairFit"
bpy.ops.wm.open_mainfile(filepath=str(OUT / "Romol_Soch.blend"))
hair = bpy.data.objects["Soch_PastTugun"]
rom = bpy.data.objects["Romol"]
arm = bpy.data.objects["ARM"]
N, R, KEEP = 160, 25, 5          # build_hair.py dagi halqa to'ri; pastdan KEEP qator qoladi
bm = bmesh.new(); bm.from_mesh(hair.data); bm.verts.ensure_lookup_table()
kill = [bm.verts[i] for i in range(KEEP * N, R * N + 1)]   # yuqori qatorlar + tepa qutbi
bmesh.ops.delete(bm, geom=kill, context="VERTS")
# Qolgan lentaning yuqori chetini bosh terisiga biroz tortamiz (ro'mol ichida yashirin bo'lsin)
C = Vector((0, .7, 163.8))
for v in bm.verts:
    if v.index < (KEEP) * N and v.index >= (KEEP - 2) * N:
        d = v.co - C
        v.co = C + d * (1 - 0.006)
dg = bpy.context.evaluated_depsgraph_get()
e = rom.evaluated_get(dg); m = e.to_mesh()
rb = BVHTree.FromPolygons([rom.matrix_world @ v.co for v in m.vertices], [tuple(p.vertices) for p in m.polygons])
e.to_mesh_clear()
head = bpy.data.objects["Bosh"]
hm = head.data
hb = BVHTree.FromPolygons([head.matrix_world @ v.co for v in hm.vertices], [tuple(p.vertices) for p in hm.polygons])
# Ro'mol ostidagi lenta nuqtalarini ro'moldan kamida 0.3 sm ichkariga (lekin teridan 0.05 sm tashqarida) qisamiz
mw = hair.matrix_world; mi = mw.inverted(); moved = 0
for v in bm.verts:
    if v.index >= KEEP * N:   # tugun (eski indekslar) — tegmaymiz
        continue
    p = mw @ v.co; d = (p - C).normalized()
    hit, *_ = rb.ray_cast(C + d * 40, -d, 40)
    if hit is None:
        continue
    lim = (hit - C).length - 0.3
    if (p - C).length > lim:
        sk, *_ = hb.ray_cast(C + d * 40, -d, 40)
        lo = (sk - C).length + 0.05 if sk is not None else 0
        v.co = mi @ (C + d * max(lim, lo)); moved += 1
print("clamped", moved)
bm.to_mesh(hair.data); bm.free()
print("hair verts", len(hair.data.vertices))
gap = []
for v in hair.data.vertices:
    p = hair.matrix_world @ v.co; d = (p - C).normalized()
    hit, *_ = rb.ray_cast(C + d * 40, -d, 40)
    if hit is not None:
        gap.append(((hit - C).length - (p - C).length, round(p.z, 1)))
inside = [g for g in gap if g[0] > 0]
print("hair under scarf: min gap %.2f cm, verts %d" % (min(g[0] for g in inside), len(inside)))
bpy.context.scene.unit_settings.scale_length = .01
for o in bpy.context.selected_objects: o.select_set(False)
hair.select_set(True); arm.select_set(True)
bpy.context.view_layer.objects.active = arm
arm.name = "root"
bpy.ops.export_scene.fbx(filepath=str(OUT / "SK_Soch_PastTugun.fbx"), use_selection=True,
                         object_types={"ARMATURE", "MESH"}, apply_unit_scale=True,
                         apply_scale_options="FBX_SCALE_NONE", add_leaf_bones=False, bake_anim=False,
                         mesh_smooth_type="FACE", path_mode="STRIP")
arm.name = "ARM"
bpy.context.preferences.filepaths.save_version = 0
bpy.ops.wm.save_as_mainfile(filepath=str(OUT / "Romol_Soch_Trim.blend"))
print("TRIMMED HAIR EXPORTED")
