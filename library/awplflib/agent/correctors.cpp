#include "correctors.h"



using namespace agent;


void	TBaselineCorrector::Process(const TMatrix& data, std::vector<int>& corrections) {
	// Process corrections and push [0, 1] to corrections.
	// 0 - No Corrections Needed
	// 1 - Correct result

	// TODO: make multithreaded
	projected_.Resize(1, pca_proj_.cols());
	corrections.resize(data.rows());
	for (size_t r = 0; r < data.rows(); ++r) {
		const float* data_row = data.GetRow(r);
		float* proj_row = projected_.GetRow(0);

		// proj = (data - centre) * pca_proj;
		for (size_t p = 0; p < pca_proj_.cols(); ++p) {
			float proj_value = 0;
			for (size_t f = 0; f < pca_proj_.rows(); ++f) {
				const float* proj_row = pca_proj_.GetRow(f);
				proj_value += (data_row[f] - center_[f]) * proj_row[p];
			}
			proj_row[p] = proj_value;
		}

		// Distance to clusters;
		size_t best_centroid = 0;
		float best_dist = FLT_MAX;
		for (size_t c = 0; c < centroids_.size(); ++c) {
			const auto& centroid = centroids_[c];
			float dist = 0;
			for (size_t p = 0; p < pca_proj_.cols(); ++p) {
				dist += std::powf(centroid[p] - proj_row[p], 2);
			}

			if (best_dist > dist) {
				best_centroid = c;
				best_dist = dist;
			}
		}

		// Project threshold;
		float thres_proj = 0;
		for (size_t p = 0; p < pca_proj_.cols(); ++p) {
			thres_proj += fisher_.GetRow(p)[best_centroid] * proj_row[p];
		}

		corrections[r] = (thres_[best_centroid] >= thres_proj) ? 1 : 0;
	}

}

bool TBaselineCorrector::LoadXML(TiXmlElement* node) {
	if (TCorrectorBase::LoadXML(node)) {

		// Load center shift
		auto center_node = node->FirstChildElement("centre");
		if (!center_node) {
			return false;
		}

		center_ = split_vector<float>(center_node->GetText());

		size_t feats_size = center_.size();

		auto centroids_node = node->FirstChildElement("centroids");
		if (!centroids_node) {
			return false;
		}

		size_t latent_size = 0;

		// Load centroids;
		for (TiXmlElement* child = centroids_node->FirstChildElement("centroid"); child; child = child->NextSiblingElement("centroid")) {

			auto centroid = split_vector<float>(child->GetText());
			if (!latent_size) {
				latent_size = centroid.size();
			}
			else if (latent_size != centroid.size())
				return false;

			centroids_.emplace_back(std::move(centroid));
		}

		pca_proj_.Resize(feats_size, latent_size);

		auto project_node = node->FirstChildElement("project");
		if (!project_node) {
			return false;
		}

		// Load pca_projection
		size_t proj_row = 0;
		for (TiXmlElement* child = project_node->FirstChildElement("row"); child; child = child->NextSiblingElement("row"), proj_row++) {

			if (!split_vector<float>(child->GetText(), pca_proj_.GetRow(proj_row), latent_size + 1))
				return false;
		}

		fisher_.Resize(latent_size, centroids_.size());

		auto fd_node = node->FirstChildElement("FD");
		if (!fd_node) {
			return false;
		}
		proj_row = 0;
		for (TiXmlElement* child = fd_node->FirstChildElement("row"); child; child = child->NextSiblingElement("row"), proj_row++) {

			if (!split_vector<float>(child->GetText(), fisher_.GetRow(proj_row), centroids_.size() + 1))
				return false;
		}

		auto treshs_node = node->FirstChildElement("treshs");
		if (!treshs_node) {
			return false;
		}

		thres_ = split_vector<float>(treshs_node->GetText());

		if (thres_.size() != centroids_.size())
			return false;


		return true;
	}

	return false;
}

TiXmlElement* TBaselineCorrector::SaveXML() {

	auto node = TCorrectorBase::SaveXML();

	node->SetValue("TBaselineCorrector");

	auto center_node = new TiXmlElement("centre");

	center_node->LinkEndChild(new TiXmlText(serialize_vector(center_)));

	node->LinkEndChild(center_node);

	auto centroids_node = new TiXmlElement("centroids");

	for (const auto& centroid : centroids_) {
		auto centroid_node = new TiXmlElement("centroid");
		centroid_node->LinkEndChild(new TiXmlText(serialize_vector(centroid)));
		centroids_node->LinkEndChild(centroid_node);
	}
	node->LinkEndChild(centroids_node);

	auto project_node = new TiXmlElement("project");

	for (size_t r = 0; r < pca_proj_.rows(); ++r) {
		auto row_node = new TiXmlElement("row");
		row_node->LinkEndChild(new TiXmlText(serialize_vector(pca_proj_.GetRow(r), pca_proj_.cols())));
		project_node->LinkEndChild(row_node);
	}

	node->LinkEndChild(project_node);

	auto fd_node = new TiXmlElement("FD");

	for (size_t r = 0; r < fisher_.rows(); ++r) {
		auto row_node = new TiXmlElement("row");
		row_node->LinkEndChild(new TiXmlText(serialize_vector(fisher_.GetRow(r), fisher_.cols())));
		fd_node->LinkEndChild(row_node);
	}
	node->LinkEndChild(fd_node);

	auto thres_node = new TiXmlElement("treshs");
	thres_node->LinkEndChild(new TiXmlText(serialize_vector(thres_)));
	node->LinkEndChild(thres_node);


	return node;
}