import os

awplf_root_path = '\\'.join(os.path.dirname(os.path.realpath(__file__)).split('\\')[0:-1])+ '\\'
#awplf_root_path = os.path.dirname(os.path.realpath(__file__)) + '\\..\\'
awplf_bin_path = awplf_root_path + 'build\\bin\\x64\\Release\\' 

if not "3rdparty/Boost/lib/" in os.environ["PATH"]:
    library_path = awplf_root_path + '3rdparty\\Boost\\lib\\' 
    os.environ["PATH"] += library_path
#    os.add_dll_directory(library_path)
script_path = awplf_root_path + 'library\\awplflib\\scripts'
os.environ["pyscript_path"] = script_path

#os.system('PATH')

class SupervisorCorrectors:
    def __init__(self, db_path, corrs_path=None, agent_path=None,
            detector_path=None, save_path=None, detector_gpu_path=None):
        self.db_path = db_path
        self.corrs_path = corrs_path
        self.agent_path = agent_path
        self.detector_path = detector_path
        self.save_path = save_path
        self.detector_gpu_path = detector_gpu_path

    def train(self, overlap=0.5, gpu=False):
        return not os.system(awplf_bin_path + 'supervisor_correctors train ' + 
                  (f'--det={self.detector_path} ' if not gpu else f'--det={self.detector_gpu_path} ') +
                  (f'--save_path={self.save_path}' if self.save_path else '') + f' --db_folder={self.db_path} ' + 
                  f'--overlap={overlap}' + 
                  (f' --corrs_path={self.corrs_path}' if self.corrs_path else ''))

    def test(self, overlap=0.5):
        return not os.system(awplf_bin_path + f'supervisor_correctors test --agent={self.agent_path} ' +
                  f'--db_folder={self.db_path} --overlap={overlap}' +
                  (f' --corrs_path={self.corrs_path}' if self.corrs_path else ''))


class TreeBuilder:
    def __init__(self, tree_engine_path, tree_paths, detector_path=None, agent_path=None):
        self.tree_engine_path = tree_engine_path
        self.tree_paths = tree_paths
        self.detector_path = detector_path
        self.agent_path = agent_path

    def export(self, use_agent=True):
        if len(self.tree_paths) != 1:
            raise Exception("Only one tree path could be exported")

        return not os.system(awplf_bin_path + f'tree_builder export ' +
                  f'--tree={self.tree_engine_path} ' +
                  (f'--agent={self.agent_path} ' if use_agent else f'--det={self.detector_path} ') +
                  f'--tree_path={self.tree_paths[0]}')

    def merge(self, use_agent=True):
        return not os.system(awplf_bin_path + f'tree_builder merge ' + 
                  f'--tree={self.tree_engine_path} ' + 
                  (f'--agent={self.agent_path} ' if use_agent else f'--det={self.detector_path} ') + 
                  ' '.join(f'--tree_path={x}' for x in self.tree_paths))

class CSBuilder:
    def __init__(self, xml_path):
        self.xml_path = xml_path

    def info(self):
        return not os.system(awplf_bin_path + f'csbuilder -i {self.xml_path}')

    def add(self):
        return not os.system(awplf_bin_path + f'csbuilder -a {self.xml_path}')

    def build(self):
        return not os.system(awplf_bin_path + f'csbuilder -b {self.xml_path}')
