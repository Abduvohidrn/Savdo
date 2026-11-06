"""Ro'mol ostidagi soch, 2-versiya: tabiiy soch chizig'i.
- soch chizig'i: o'rtada kichik uchi, ko'z qirralari ustida biroz ko'tarilgan burchaklar, chakkada quloqqa tushadi
- qalinlik chetda ~0 ga tushadi (tasma pog'onasi yo'q)
- UV0.x - o'rtadagi farqdan taraladigan tola yo'nalishi, UV0.y - chetdan masofa (0 chiziqda, 1 ichkarida)
- uchala Citizen ayol boshi (02_asian, 02_b, 01) terisidan tashqarida, ro'moldan 0.3 sm ichkarida
- ensadagi tugun eski Romol_Soch.blend'dan olinadi
blender -b --factory-startup --python build_hair_v2.py
"""
import bpy, bmesh, math
from pathlib import Path
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree

P = Path(r"D:/Unreal Projects/BozorUnreal")
OUT = P / "Assets/Romol/HairFit"
HEADS = Path(r"C:/Users/A5378~1.RAY/AppData/Local/Temp/claude/489816a719b2dd10/70da2178-0b90-4cd1-ade5-bdda3d476dce/scratchpad")
bpy.ops.wm.open_mainfile(filepath=str(OUT / "Romol_Soch.blend"))
sc = bpy.context.scene
bosh, rom, arm, old = (bpy.data.objects[n] for n in ("Bosh", "Romol", "ARM", "Soch_PastTugun"))
C = Vector((0, .7, 163.8))
dg = bpy.context.evaluated_depsgraph_get()


def world_mesh(o):
    e = o.evaluated_get(dg); m = e.to_mesh()
    vs = [o.matrix_world @ v.co for v in m.vertices]; fs = [tuple(p.vertices) for p in m.polygons]
    e.to_mesh_clear(); return vs, fs


def bbox(vs):
    lo = Vector([min(v[i] for v in vs) for i in range(3)]); hi = Vector([max(v[i] for v in vs) for i in range(3)])
    return (lo + hi) / 2, (hi - lo) / 2


# ---- boshlar: Blender'dagi Bosh (02_asian) + Unreal'dan eksport qilingan uchtasi
bvs, bfs = world_mesh(bosh)
bc, be = bbox(bvs)
head_bvh = [BVHTree.FromPolygons(bvs, bfs)]
imported = {}
for n in ("SK_fe_head_02_asian", "SK_fe_head_02_b", "SK_fe_head_01"):
    before = set(bpy.data.objects)
    bpy.ops.import_scene.fbx(filepath=str(HEADS / (n + ".fbx")))
    meshes = [o for o in bpy.data.objects if o not in before and o.type == "MESH"]
    vs, fs = [], []
    for o in meshes:
        v2, f2 = world_mesh(o); off = len(vs)
        vs += v2; fs += [tuple(i + off for i in f) for f in f2]
    imported[n] = (vs, fs)
# FBX importi masshtab/o'qlarni Bosh bilan solishtirib tekislash (02_asian = Bosh)
av, _ = imported["SK_fe_head_02_asian"]
ac, ae = bbox(av)
print("raw asian", bbox(av), "bosh", bc, be)
best = None
for flip in (1, -1):
    for swap in (False, True):
        for s in (1.0, 100.0, 0.01):
            def tf(v, flip=flip, swap=swap, s=s):
                x, y, z = (v.x, v.z, -v.y) if swap else (v.x, v.y, v.z)
                return Vector((x, flip * y, z)) * s
            tv = [tf(v) for v in av]
            c2, e2 = bbox(tv)
            err = (e2 - be).length
            if best is None or err < best[0]:
                best = (err, tf, bc - c2)
err, tf0, off = best
# uchlar bo'yicha aniq moslash: Bosh uchlariga eng yaqin nuqtalar o'rtacha farqi
tf = lambda v: tf0(v) + off
from mathutils.kdtree import KDTree
kd = KDTree(len(bvs))
for k, v in enumerate(bvs): kd.insert(v, k)
kd.balance()
res = [kd.find(tf(v))[2] for v in av[::5]]
print("head align ext err %.3f off %s  mean vertex dist %.4f max %.4f" % (err, tuple(round(x, 3) for x in off), sum(res) / len(res), max(res)))
for n, (vs, fs) in imported.items():
    wv = [tf(v) for v in vs]
    c2, e2 = bbox(wv)
    print(n, "center", tuple(round(x, 2) for x in c2), "ext", tuple(round(x, 2) for x in e2))
    head_bvh.append(BVHTree.FromPolygons(wv, fs))
for o in list(bpy.data.objects):
    if o.name not in ("Bosh", "Romol", "ARM", "Soch_PastTugun", "Tana", "Kiyim", "Beanie") and o.type in ("MESH", "ARMATURE", "EMPTY"):
        bpy.data.objects.remove(o)

rvs, rfs = world_mesh(rom)
rom_bvh = BVHTree.FromPolygons(rvs, rfs)


def dirv(a, th):
    return Vector((math.sin(th) * math.sin(a), -math.sin(th) * math.cos(a), math.cos(th)))


def skin_r(d):
    r = 0.0
    for t in head_bvh:
        h = t.ray_cast(C + d * 40, -d, 40)
        if h[0] is not None: r = max(r, (h[0] - C).length)
    return r


def scarf_r(d):
    h = rom_bvh.ray_cast(C + d * 40, -d, 40)
    return (h[0] - C).length if h[0] is not None else None


# ---- ro'mol chetining burchagi (yuqoridan, gradusda) har bir azimut uchun
N = 240
hem = []
for i in range(N):
    a = 2 * math.pi * i / N
    th_ok = 20.0
    for k in range(40, 260):
        th = k * 0.5
        if scarf_r(dirv(a, math.radians(th))) is None: break
        th_ok = th
    hem.append(th_ok)


def vis(adeg):
    """ro'mol chetidan pastda ko'rinadigan soch (gradus): 0 - old o'rta, 90 - quloq usti, 180 - orqa"""
    A = abs(adeg)
    pts = [(0, 7.0), (7, 6.2), (25, 4.6), (45, 9.0), (65, 15.0), (85, 15.5), (105, 8.0), (125, 1.5), (140, -3.0), (180, -4.0)]
    for (a0, v0), (a1, v1) in zip(pts, pts[1:]):
        if A <= a1:
            t = (A - a0) / (a1 - a0); t = t * t * (3 - 2 * t)
            return v0 + (v1 - v0) * t
    return pts[-1][1]


edge = []
for i in range(N):
    adeg = (360 * i / N + 180) % 360 - 180
    e = hem[i] + vis(adeg)
    e += 0.9 * math.exp(-(adeg / 4.0) ** 2)          # kichik uchi (widow's peak)
    e += 0.35 * math.sin(i * 1.7) + 0.25 * math.sin(i * 4.3)   # chiziq juda tekis bo'lmasin
    if abs(adeg) < 40: e = min(e, 74.0)                # qoshgacha tushmasin
    edge.append(e)
# yumshatish
for _ in range(3):
    edge = [(edge[i - 1] + 2 * edge[i] + edge[(i + 1) % N]) / 4 for i in range(N)]
print("hem front %.1f side %.1f back %.1f | edge front %.1f side %.1f" % (hem[0], hem[N // 4], hem[N // 2], edge[0], edge[N // 4]))

# ---- qobiq
R, STEP = 28, 1.15
vs, uvs_v = [], []
for j in range(R):
    for i in range(N):
        a = 2 * math.pi * i / N
        th = edge[i] - j * STEP
        d = dirv(a, math.radians(th))
        t = min(1.0, (j * STEP) / 3.0)          # qalinlik ~0.6 sm da to'liq bo'ladi
        t = t * t * (3 - 2 * t)
        rs = skin_r(d)
        r = rs + 0.035 + 0.26 * t
        t = min(1.0, (j * STEP) / 1.6)          # UV.y: chet niqobi ~0.3 sm da tugaydi
        rc = scarf_r(d)
        if rc is not None: r = min(r, rc - 0.3)
        r = max(r, rs + 0.03)
        p = C + d * r
        vs.append(p)
        uvs_v.append(t)
fs = []
for j in range(R - 1):
    for i in range(N):
        fs.append((j * N + i, (j + 1) * N + i, (j + 1) * N + (i + 1) % N, j * N + (i + 1) % N))
# tugun: eski mesh'dagi 25*160+1 dan keyingi uchlar
O_N, O_R = 160, 25
ov = old.data.vertices
base = len(vs)
bun_ids = list(range(O_R * O_N + 1, len(ov)))
remap = {}
for k, vi in enumerate(bun_ids):
    remap[vi] = base + k
    vs.append(old.matrix_world @ ov[vi].co); uvs_v.append(1.0)
for p in old.data.polygons:
    if all(v in remap for v in p.vertices):
        fs.append(tuple(remap[v] for v in p.vertices))

me = bpy.data.meshes.new("Soch_TabiiyChiziq")
me.from_pydata(vs, [], fs)
uv = me.uv_layers.new(name="UVMap")
for poly in me.polygons:
    poly.use_smooth = True
    for li in poly.loop_indices:
        vi = me.loops[li].vertex_index
        p = vs[vi]
        u = math.atan2(p.x, p.z - C.z) / (2 * math.pi) + 0.5   # o'rtadagi farqdan taraladi
        uv.data[li].uv = (u, uvs_v[vi])
bm = bmesh.new(); bm.from_mesh(me)
bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
out_dot = sum((f.normal.dot((f.calc_center_median() - C).normalized())) for f in bm.faces[:N * (R - 1)])
if out_dot < 0:
    for f in bm.faces: f.normal_flip()
bm.to_mesh(me); bm.free()

hair = bpy.data.objects.new("Soch_PastTugun_v2", me)
sc.collection.objects.link(hair)
mat = bpy.data.materials.new("M_Soch_PastTugun")
mat.use_nodes = True
nt = mat.node_tree; bs = next(n for n in nt.nodes if n.type == "BSDF_PRINCIPLED")
bs.inputs["Base Color"].default_value = (.02, .011, .007, 1); bs.inputs["Roughness"].default_value = .5
me.materials.append(mat)
hair.vertex_groups.new(name="head").add(list(range(len(me.vertices))), 1, "REPLACE")
hair.parent = arm; hair.matrix_parent_inverse = arm.matrix_world.inverted()
hair.modifiers.new("Citizen head", "ARMATURE").object = arm
old.hide_render = True; old.hide_set(True)

# tekshiruv: har boshdan tashqarida, ro'mol ostida
worst_skin, worst_scarf = 9, 9
for p in vs[:N * R]:
    d = (p - C).normalized(); r = (p - C).length
    worst_skin = min(worst_skin, r - skin_r(d))
    rc = scarf_r(d)
    if rc is not None: worst_scarf = min(worst_scarf, rc - r)
print("min skin gap %.3f  min scarf gap %.3f  verts %d" % (worst_skin, worst_scarf, len(vs)))

sc.unit_settings.scale_length = .01
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
bpy.ops.wm.save_as_mainfile(filepath=str(OUT / "Romol_Soch_v2.blend"))
print("HAIR V2 EXPORTED")
