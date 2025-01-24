import pickle
import sys
import warnings

warnings.filterwarnings("ignore")

from create_class_corr3 import SOMCorrector

if __name__ == "__main__":
    path_to_data = sys.argv[1]
    with open(path_to_data, "rb") as f:
        data = pickle.load(f)
    em = SOMCorrector(map_size=(10, 10))
    em.load_data(X=data["features"], y=data["target"], test_size=0.2)
    em.fit()
    em.evaluate()
