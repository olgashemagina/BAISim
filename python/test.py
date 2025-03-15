from awplflib import SupervisorCorrectors as sc
from awplflib import TreeBuilder as tb
from awplflib import CSBuilder as builder

a = sc(db_path="C:/data/digits/0/test/", agent_path="C:/work/BAISim/tools/supervisor_correctors/vc/vc2019/agent_digit0.xml", detector_path="C:/data/digits/0_075_v3.xml")
if(a.test()):
    print("Test passed!")
if(a.train()):
    print("Test passed!")

b = tb(tree_engine_path="rw_tree.xml", detector_path="C:/data/railnumber/rnum_cs.xml", tree_paths=("Number",))
if(b.merge(use_agent=False)):
    print("Test passed!")
if(b.export(use_agent=False)):
    print("Test passed!")

c = builder("C:/data/digits/csbuild_0.xml")
if(c.build()):
    print("Test passed!")
if(c.info()):
    print("Test passed!")
if(c.add()):
    print("Test passed!")
