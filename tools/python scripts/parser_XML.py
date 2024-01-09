from lxml import etree as ET
import pandas as pd

def parse_xml(path : str) -> pd.DataFrame:

    '''
    input:
    
    path : str --- path to xml file to parse

    output:

    data : pd.Dataframe --- pandas DataFrame with parsed XML file

    '''

    data = pd.read_xml(path)
    tree = ET.parse(path)
    lfsamples = tree.getroot()
    res = []
    for lfsampledescriptor in lfsamples:
        features = []
        for lfstagedescriptor in lfsampledescriptor:
            strong = []
            elem = lfstagedescriptor.attrib
            for lfweakdescriptor in lfstagedescriptor:
                strong.append((elem,{'attributes':lfweakdescriptor.attrib,'features':lfweakdescriptor.find('./LFFeatureDescriptor').text.split(' ')}))
            features.append(strong)
        res.append(features)
    data['LFStageDescriptor']=res

    return data
