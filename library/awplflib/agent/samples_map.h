#pragma once
#include <vector>

// Map of samples in linear array (or matrix)
class SamplesMap {
public:
	void		Setup(size_t ids_count) {
		is_stopped_.clear();
		free_.clear();
		is_stopped_.resize(ids_count, false);

		for (auto& queue : queues_) {
			queue.clear();
		}
		queues_.resize(ids_count);
	}

	// Mark id is stopped for adding;
	void			Stop(size_t id) {
		if (!is_stopped_[id]) {
			is_stopped_[id] = true;
			free_.insert(free_.end(), queues_[id].begin(), queues_[id].end());
		}
	}

	bool			IsStopped(size_t id) const {
		return is_stopped_[id];
	}

	// Add used place position for id;
	void			Push(size_t id, size_t place) {
		queues_[id].push_back(place);
	}

	// Check if there is a free place
	size_t			Pop() {
		if (free_.empty())
			return -1;

		size_t place = free_.back();
		free_.pop_back();
		return place;
	}

	// Get All stopped places
	std::vector<size_t>& GetFree() {
		return free_;
	}

private:
	// Places where detections ids are
	std::vector<std::vector<size_t>>		queues_;

	// Stop gathering detections with this id;
	std::vector<bool>					is_stopped_;
	std::vector<size_t>					free_;
};

