#include <registration.hpp>

namespace {
	using namespace LUA_MODULE_NAME;
	using namespace cvextra;
	using byte = unsigned char;

	template<typename... Args>
	inline MatIndexType mat_at(lua_State* L, const cv::Mat& m, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			return m.at<uchar>(std::forward<Args>(args)...);
		case CV_8S:
			return m.at<char>(std::forward<Args>(args)...);
		case CV_16U:
			return m.at<ushort>(std::forward<Args>(args)...);
		case CV_16S:
			return m.at<short>(std::forward<Args>(args)...);
		case CV_32S:
			return m.at<int>(std::forward<Args>(args)...);
		case CV_32F:
			return m.at<float>(std::forward<Args>(args)...);
		case CV_64F:
			return m.at<double>(std::forward<Args>(args)...);
		default:
			LUAL_MODULE_ERROR_RETURN(L, "Unsupported matrix type");
		}
	}

	template<typename... Args>
	inline void mat_set_at(lua_State* L, cv::Mat& m, double value, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			m.at<uchar>(std::forward<Args>(args)...) = static_cast<uchar>(value);
			break;
		case CV_8S:
			m.at<char>(std::forward<Args>(args)...) = static_cast<char>(value);
			break;
		case CV_16U:
			m.at<ushort>(std::forward<Args>(args)...) = static_cast<ushort>(value);
			break;
		case CV_16S:
			m.at<short>(std::forward<Args>(args)...) = static_cast<short>(value);
			break;
		case CV_32S:
			m.at<int>(std::forward<Args>(args)...) = static_cast<int>(value);
			break;
		case CV_32F:
			m.at<float>(std::forward<Args>(args)...) = static_cast<float>(value);
			break;
		case CV_64F:
			m.at<double>(std::forward<Args>(args)...) = value;
			break;
		default:
			LUAL_MODULE_ERROR(L, "Unsupported matrix type");
		}
	}

	inline bool isScalar(const MatIndexType& value) {
		if (std::holds_alternative<uchar>(value)) {
			return true;
		}

		if (std::holds_alternative<char>(value)) {
			return true;
		}

		if (std::holds_alternative<ushort>(value)) {
			return true;
		}

		if (std::holds_alternative<short>(value)) {
			return true;
		}

		if (std::holds_alternative<int>(value)) {
			return true;
		}

		if (std::holds_alternative<float>(value)) {
			return true;
		}

		if (std::holds_alternative<double>(value)) {
			return true;
		}

		return false;
	}

	inline double getScalar(const MatIndexType& value) {
		if (std::holds_alternative<uchar>(value)) {
			return static_cast<double>(std::get<uchar>(value));
		}

		if (std::holds_alternative<char>(value)) {
			return static_cast<double>(std::get<char>(value));
		}

		if (std::holds_alternative<ushort>(value)) {
			return static_cast<double>(std::get<ushort>(value));
		}

		if (std::holds_alternative<short>(value)) {
			return static_cast<double>(std::get<short>(value));
		}

		if (std::holds_alternative<int>(value)) {
			return static_cast<double>(std::get<int>(value));
		}

		if (std::holds_alternative<float>(value)) {
			return static_cast<double>(std::get<float>(value));
		}

		if (std::holds_alternative<double>(value)) {
			return static_cast<double>(std::get<double>(value));
		}

		return 0.0;
	}

	inline void setRangeShape(lua_State* L, std::vector<cv::Range>& ranges, std::vector<int>& newshape, int size, const std::vector<std::variant<int, cv::Range>>& idx, int i) {
		if (std::holds_alternative<cv::Range>(idx[i])) {
			const auto& range = std::get<cv::Range>(idx[i]);
			if (range.empty()) {
				LUAL_MODULE_ERROR(L, "range at index " << i << " is empty");
			}
			newshape.push_back(range == cv::Range::all() || range.size() >= size ? size : range.size());
			ranges[i] = range;
		}
		else {
			auto start = std::get<int>(idx[i]);
			ranges[i] = cv::Range(start, start + 1);
		}
	}

	inline cv::Mat check_reshape(lua_State* L, cv::Mat& res, std::vector<int>& newshape, const std::vector<std::variant<int, cv::Range>>& idx) {
		if (!res.isContinuous()) {
			LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
		}

		// there is at least one dimension that is not required
		// reshape to keep only the required dimensions
		return res.reshape(0, newshape);
	}

	inline void setTo(cv::Mat& m, const MatIndexType& value) {
		if (std::holds_alternative<cv::Mat>(value)) {
			m.setTo(std::get<cv::Mat>(value));
		}
		else {
			m.setTo(getScalar(value));
		}
	}

	template<typename... Args>
	inline void setScalar(lua_State* L, cv::Mat& self, const MatIndexType& value, Args&&... args) {
		if (!std::holds_alternative<cv::Mat>(value)) {
			::mat_set_at(L, self, getScalar(value), std::forward<Args>(args)...);
			return;
		}

		const auto& m = std::get<cv::Mat>(value);
		if (m.channels() * m.total() != 1) {
			LUAL_MODULE_ERROR(L, "value must be a scalar");
		}
		::mat_set_at(L, self, cvextra::mat_at(m, 0), std::forward<Args>(args)...);
	}

	template<typename _Tp>
	inline const std::string join(_Tp* elements, size_t size, const std::string& separator = ",") {
		std::ostringstream o;
		for (size_t i = 0; i < size; i++) {
			if (i != 0) {
				o << separator;
			}
			o << elements[i];
		}
		return o.str();
	}

	void setMinMax(lua_Number value, std::vector<double>& values, std::vector<int>& indexes) {
		double value_t = value;
		values.push_back(value_t);
	}

	void traverse_matrix(
		lua_State* L,
		std::vector<double>& values,
		std::vector<int>& sizes,
		std::vector<int>& indexes,
		int dim = 0
	) {
		auto index = lua_gettop(L);
		auto size = lua_rawlen(L, index);

		if (sizes.size() <= dim) {
			sizes.push_back(size);
			indexes.push_back(0);
		}
		else if (sizes[dim] != size) {
			LUA_MODULE_THROW("all arrays must have the same size");
		}

		for (int i = 0; i < size; i++) {
			indexes[dim] = i;

			lua_rawgeti(L, index, i + 1);

			if (lua_type(L, -1) == LUA_TNUMBER) {
				setMinMax(lua_tonumber(L, -1), values, indexes);
			}
			else {
				traverse_matrix(L, values, sizes, indexes, dim + 1);
			}

			lua_pop(L, 1);
		}

		// reset index before processing another dimension
		indexes[dim] = 1;
	}

	template<typename _Tm>
	inline void _createFromArray(lua_State* L, const Table& array, int type, _Tm& result) {
		std::vector<double> values;
		std::vector<int> sizes;
		std::vector<int> indexes;

		lua_push(L, array);
		traverse_matrix(L, values, sizes, indexes);
		lua_pop(L, 1);

		if (type == -1) {
			type = CV_64F;
		}

		auto flags = CV_MAT_TYPE(type);
		auto cn = CV_MAT_CN(flags);
		auto depth = CV_MAT_DEPTH(flags);

		cv::Mat data(sizes, CV_64F, reinterpret_cast<void*>(values.data()));

		data.convertTo(result, depth);

		if (cn != 1) {
			auto dims = sizes.size();
			if (cn == sizes.back()) {
				result = result.reshape(cn, dims - 1, result.size.p);
			}
			else {
				LUAL_MODULE_ERROR(L, "the given array has " << sizes.back() << " channels, while given type has " << cn << " channels");
			}
		}
	}

	template<typename _Tp, typename _Tm>
	inline int _pushtable(lua_State* L, const _Tm& _self, bool nested) {
		lua_newtable(L);
		if (_self.empty()) {
			return 1;
		}

		const cv::Mat self = ([&] {
			if constexpr (std::is_same_v<_Tm, cv::UMat>) {
				return _self.getMat(cv::ACCESS_READ);
			}
			else {
				return _self;
			}
			}());

		const auto channels = self.channels();
		const auto total = self.total() * channels;
		auto dims = self.dims;

		if (self.cols == 1) {
			dims--;
		}

		if (channels != 1) {
			dims++;
		}

		std::vector<int> sizes(dims);
		std::vector<size_t> steps(dims);
		std::vector<int> indexes(dims, 0);

		for (int i = 0; i < dims; i++) {
			auto j = i == 0 || self.cols != 1 ? i : i + 1;
			sizes[i] = j < self.dims ? self.size[j] : channels;
			steps[i] = j < self.dims ? self.step[j] : sizeof(_Tp);
		}

		std::vector<int> ssizes(dims, 1);
		for (int i = dims - 2; i >= 0; i--) {
			ssizes[i] = ssizes[i + 1] * sizes[i + 1];
		}

		const uchar* data = self.ptr();

		for (uint64_t i = 0, j = 0; i < total; i++) {
			while (indexes[j] == sizes[j]) {
				if (nested) {
					lua_pop(L, 1);
				}
				indexes[j--] = 0;
				indexes[j]++;
			}

			if (j != dims - 1) {
				if (nested) {
					int index = 1 + indexes[j];
					lua_newtable(L);
					lua_rawseti(L, -2, index);

					// lua_rawseti will pop the table from the stack
					// added it back onto the stack
					// because the top of the stack is the table we are filling
					lua_rawgeti(L, -1, index);
				}

				i--;
				j++;
				continue;
			}

			size_t offset = 0;
			size_t index = i;
			size_t nexti = i;

			for (int k = 0; k < dims; k++) {
				index = nexti / ssizes[k];
				offset += index * steps[k];
				nexti %= ssizes[k];
			}

			lua_push(L, *reinterpret_cast<const _Tp*>(data + offset));

			if (nested) {
				lua_rawseti(L, -2, 1 + indexes[j]);
			}
			else {
				lua_rawseti(L, -2, 1 + i);
			}

			indexes[j]++;
		}

		if (nested) {
			// for j = 0; j < dims - 1; j++ lua_newtable
			// there were dims - 1 tables pushed onto the stack
			// remove them from the stack
			lua_pop(L, dims - 1);
		}

		return 1;
	}

	template<typename _Tm>
	inline int _pushtable(lua_State* L, const _Tm& self, bool nested) {
		switch (self.depth()) {
		case CV_8U:
			return _pushtable<byte, _Tm>(L, self, nested);
		case CV_8S:
			return _pushtable<char, _Tm>(L, self, nested);
		case CV_16U:
			return _pushtable<ushort, _Tm>(L, self, nested);
		case CV_16S:
			return _pushtable<short, _Tm>(L, self, nested);
		case CV_32S:
			return _pushtable<int, _Tm>(L, self, nested);
		case CV_32F:
			return _pushtable<float, _Tm>(L, self, nested);
		case CV_64F:
			return _pushtable<double, _Tm>(L, self, nested);
		default:
			LUAL_MODULE_ERROR_RETURN(L, "depth must be one of CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F");
		}
	}
}

namespace cvextra {
	cv::Range Ellipsis;

	std::vector<int> mat_shape(const cv::Mat& self) {
		const auto dims = self.size.dims();
		std::vector<int> shape(self.size.p, self.size.p + dims);
		const auto channels = self.channels();
		if (channels != 1) {
			shape.push_back(channels);
		}
		return shape;
	}

	std::vector<int> umat_shape(const cv::UMat& self) {
		const auto dims = self.size.dims();
		std::vector<int> shape(self.size.p, self.size.p + dims);
		const auto channels = self.channels();
		if (channels != 1) {
			shape.push_back(channels);
		}
		return shape;
	}

	MatIndexType Mat_index_at(lua_State* L, cv::Mat& self, int idx) {
		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		if (dims == 1) {
			if (channels != 1) {
				// treated as dims == 2
				int newsz[] = { channels };
				return self.row(idx).reshape(1, 1, newsz);
			}

			return ::mat_at(L, self, idx);
		}

		if (dims > 2) {
			cv::Mat res = self.row(idx);
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			return res.reshape(channels, dims - 1, size.p + 1);
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			cv::Mat res = self.row(idx);
			int newsz[] = { size[1], channels };
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			return res.reshape(1, 2, newsz);
		}

		// dims == 2, channels == 1

		// opencv Mat does not support 1D matrix
		// treat a matrix with 1 channel and (1 row or 1 column) as a 1D matrix
		if (size[0] == 1 || size[1] == 1) {
			return ::mat_at(L, self, idx);
		}

		return self.row(idx);
	}

	MatIndexType Mat_index_at(lua_State* L, cv::Mat& self, const cv::Range& range) {
		std::vector<cv::Range> ranges(self.dims, cv::Range::all());
		ranges[0] = range;
		return cv::Mat(self, ranges);
	}

	MatIndexType Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx) {
		if (idx.size() == 0) {
			LUAL_MODULE_ERROR(L, "index can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (idx.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than index " << idx.size() << " dimensions");
		}

		if (dims == 1) {
			return Mat_index_at(L, self, idx[0]);
		}

		if (idx.size() == self.dims) {
			if (channels == 1) {
				return ::mat_at(L, self, idx.data());
			}

			// only channels remains

			std::vector<cv::Range> ranges(idx.size());
			for (int i = 0; i < idx.size(); i++) {
				ranges[i] = cv::Range(idx[i], idx[i] + 1);
			}

			cv::Mat res(self, ranges);

			// channels are always continuous, therefore, reshaping to channels only is always possible
			int newsz[] = { channels };
			return res.reshape(1, 1, newsz);
		}

		if (idx.size() == dims) {
			// channels is treated as a dimension

			// a full coordinate of an item in the matrix is required
			int offset = 0;
			for (int i = 0; i < self.dims; i++) {
				offset += idx[i] * self.step.p[i];
			}

			cv::Mat res(cv::Size(channels, 1), self.depth(), static_cast<void*>(self.data + offset));
			return ::mat_at(L, res, idx.back());
		}

		// idx.size() < self.dims

		std::vector<cv::Range> ranges(idx.size());
		for (int i = 0; i < idx.size(); i++) {
			ranges[i] = cv::Range(idx[i], idx[i] + 1);
		}

		cv::Mat res(self, ranges);
		std::vector<int> newshape(size.p + idx.size(), size.p + size.dims());
		return res.reshape(0, newshape);
	}

	MatIndexType Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges) {
		if (ranges.size() == 0) {
			LUAL_MODULE_ERROR(L, "ranges can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (ranges.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than ranges " << ranges.size() << " dimensions");
		}

		if (dims == 1) {
			return Mat_index_at(L, self, ranges[0]);
		}

		if (ranges.size() == self.dims) {
			return cv::Mat(self, ranges);
		}

		std::vector<int> newshape;

		for (int i = 0; i < ranges.size(); i++) {
			const auto& range = ranges[i];
			if (range.empty()) {
				LUAL_MODULE_ERROR(L, "range at index " << i << " must not be empty");
			}
			auto length = i < self.dims ? size.p[i] : channels;
			newshape.push_back(range == cv::Range::all() || range.size() >= length ? length : range.size());
		}

		if (ranges.size() == dims) {
			// channels is treated as a dimension

			if (!self.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet. Channels cannot be treated as a new dimension");
			}

			// reshape with channels added as a new dimension
			std::vector<int> newsz(size.p, size.p + size.dims());
			newsz.push_back(channels);
			return cv::Mat(self.reshape(1, newsz), ranges);
		}

		// ranges.size() < self.dims
		return cv::Mat(self, ranges);
	}

	MatIndexType Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx) {
		if (idx.size() == 0) {
			LUAL_MODULE_ERROR(L, "index can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (idx.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than index " << idx.size() << " dimensions");
		}

		if (dims == 1) {
			if (std::holds_alternative<int>(idx[0])) {
				return Mat_index_at(L, self, std::get<int>(idx[0]));
			}
			return Mat_index_at(L, self, std::get<cv::Range>(idx[0]));
		}

		std::vector<cv::Range> ranges(idx.size());
		std::vector<int> newshape;
		for (int i = 0; i < idx.size(); i++) {
			setRangeShape(L, ranges, newshape, i < self.dims ? size.p[i] : channels, idx, i);
		}

		if (idx.size() == self.dims) {
			// only channels remains
			cv::Mat res(self, ranges);
			return check_reshape(L, res, newshape, idx);
		}

		if (idx.size() == dims) {
			// channels is treated as a dimension

			if (!self.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet. Channels cannot be treated as a new dimension");
			}

			// reshape with channels added as a new dimension
			std::vector<int> newsz(size.p, size.p + size.dims());
			newsz.push_back(channels);
			cv::Mat res(self.reshape(1, newsz), ranges);

			return check_reshape(L, res, newshape, idx);
		}

		// idx.size() < self.dims
		cv::Mat res(self, ranges);
		return check_reshape(L, res, newshape, idx);
	}


	void Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const MatIndexType& value) {
		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		if (dims == 1) {
			if (channels != 1) {
				// treated as dims == 2
				int newsz[] = { channels };
				auto res = self.row(idx).reshape(1, 1, newsz);
				setTo(res, value);
				return;
			}

			setScalar(L, self, value, idx);
			return;
		}

		if (dims > 2) {
			cv::Mat res = self.row(idx);
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			res = res.reshape(channels, dims - 1, size.p + 1);
			setTo(res, value);
			return;
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			cv::Mat res = self.row(idx);
			int newsz[] = { size[1], channels };
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			res = res.reshape(1, 2, newsz);
			setTo(res, value);
			return;
		}

		// dims == 2, channels == 1

		// opencv Mat does not support 1D matrix
		// treat a matrix with 1 channel and (1 row or 1 column) as a 1D matrix
		if (size[0] == 1 || size[1] == 1) {
			setScalar(L, self, value, idx);
			return;
		}

		auto res = self.row(idx);
		setTo(res, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const cv::Range& range, const MatIndexType& value) {
		std::vector<cv::Range> ranges(self.dims, cv::Range::all());
		ranges[0] = range;
		cv::Mat res(self, ranges);
		setTo(res, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const MatIndexType& value) {
		if (idx.size() == 0) {
			LUAL_MODULE_ERROR(L, "index can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (idx.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than index " << idx.size() << " dimensions");
		}

		if (dims == 1) {
			Mat_newindex_at(L, self, idx[0], value);
			return;
		}

		if (idx.size() == self.dims) {
			if (channels == 1) {
				setScalar(L, self, value, idx.data());
				return;
			}

			// only channels remains

			std::vector<cv::Range> ranges(idx.size());
			for (int i = 0; i < idx.size(); i++) {
				ranges[i] = cv::Range(idx[i], idx[i] + 1);
			}

			cv::Mat res(self, ranges);

			// channels are always continuous, therefore, reshaping to channels only is always possible
			int newsz[] = { channels };
			res = res.reshape(1, 1, newsz);
			setTo(res, value);
			return;
		}

		if (idx.size() == dims) {
			// channels is treated as a dimension

			// a full coordinate of an item in the matrix is required
			int offset = 0;
			for (int i = 0; i < self.dims; i++) {
				offset += idx[i] * self.step.p[i];
			}

			cv::Mat res(cv::Size(channels, 1), self.depth(), static_cast<void*>(self.data + offset));
			setScalar(L, res, value, idx.back());
			return;
		}

		// idx.size() < self.dims

		std::vector<cv::Range> ranges(idx.size());
		for (int i = 0; i < idx.size(); i++) {
			ranges[i] = cv::Range(idx[i], idx[i] + 1);
		}

		cv::Mat res(self, ranges);
		std::vector<int> newshape(size.p + idx.size(), size.p + size.dims());
		res = res.reshape(0, newshape);
		setTo(res, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, const MatIndexType& value) {
		if (ranges.size() == 0) {
			LUAL_MODULE_ERROR(L, "ranges can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (ranges.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than ranges " << ranges.size() << " dimensions");
		}

		if (dims == 1) {
			Mat_newindex_at(L, self, ranges[0], value);
			return;
		}

		if (ranges.size() == self.dims) {
			auto res = cv::Mat(self, ranges);
			setTo(res, value);
			return;
		}

		std::vector<int> newshape;

		for (int i = 0; i < ranges.size(); i++) {
			const auto& range = ranges[i];
			if (range.empty()) {
				LUAL_MODULE_ERROR(L, "range at index " << i << " must not be empty");
			}
			auto length = i < self.dims ? size.p[i] : channels;
			newshape.push_back(range == cv::Range::all() || range.size() >= length ? length : range.size());
		}

		if (ranges.size() == dims) {
			// channels is treated as a dimension

			if (!self.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet. Channels cannot be treated as a new dimension");
			}

			// reshape with channels added as a new dimension
			std::vector<int> newsz(size.p, size.p + size.dims());
			newsz.push_back(channels);
			auto res = cv::Mat(self.reshape(1, newsz), ranges);
			setTo(res, value);
			return;
		}

		// ranges.size() < self.dims
		auto res = cv::Mat(self, ranges);
		setTo(res, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const MatIndexType& value) {
		if (idx.size() == 0) {
			LUAL_MODULE_ERROR(L, "index can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (idx.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than index " << idx.size() << " dimensions");
		}

		if (dims == 1) {
			if (std::holds_alternative<int>(idx[0])) {
				Mat_newindex_at(L, self, std::get<int>(idx[0]), value);
			}
			else {
				Mat_newindex_at(L, self, std::get<cv::Range>(idx[0]), value);
			}
			return;
		}

		std::vector<cv::Range> ranges(idx.size());
		std::vector<int> newshape;
		for (int i = 0; i < idx.size(); i++) {
			setRangeShape(L, ranges, newshape, i < self.dims ? size.p[i] : channels, idx, i);
		}

		if (idx.size() == self.dims) {
			// only channels remains
			cv::Mat res(self, ranges);
			res = check_reshape(L, res, newshape, idx);
			setTo(res, value);
			return;
		}

		if (idx.size() == dims) {
			// channels is treated as a dimension

			if (!self.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet. Channels cannot be treated as a new dimension");
			}

			// reshape with channels added as a new dimension
			std::vector<int> newsz(size.p, size.p + size.dims());
			newsz.push_back(channels);
			cv::Mat res(self.reshape(1, newsz), ranges);

			res = check_reshape(L, res, newshape, idx);
			setTo(res, value);
			return;
		}

		// idx.size() < self.dims
		cv::Mat res(self, ranges);
		res = check_reshape(L, res, newshape, idx);
		setTo(res, value);
	}

	cv::Mat createMatFromVectorOfMat(lua_State* L, const std::vector<cv::Mat>& vec) {
		using namespace cv;

		Mat result;
		if (vec.empty()) {
			return result;
		}

		std::vector<int> isizes(vec[0].size.p, vec[0].size.p + vec[0].dims);
		std::vector<int> sizes{ static_cast<int>(vec.size()) };
		sizes.insert(std::end(sizes), std::begin(isizes), std::end(isizes));

		auto dims = vec[0].dims;
		auto depth = vec[0].depth();
		auto cn = vec[0].channels();
		auto type = vec[0].type();;

		result = Mat::zeros(sizes.size(), sizes.data(), type);
		uchar* data = result.ptr();

		size_t i = 0;
		for (const auto& mat : vec) {
			LUAL_MODULE_ASSERT(L, mat.depth() == depth,
				"Mat at index " << i << " doesn't have the same depth as Mat at index 0");
			LUAL_MODULE_ASSERT(L, mat.channels() == cn,
				"Mat at index " << i << " doesn't have the same number of channels as Mat at index 0");
			LUAL_MODULE_ASSERT(L, mat.dims == dims,
				"Mat at index " << i << " doesn't have the same dims as Mat at index 0");
			LUAL_MODULE_ASSERT(L, std::equal(isizes.begin(), isizes.end(), mat.size.p),
				"Mat at index " << i << " doesn't have the same size as Mat at index 0");

			mat.copyTo(Mat(isizes, type, data));

			data += result.step.p[0];
			i++;
		}

		return result;
	}

	cv::Mat createMatFromArray(lua_State* L, const Table& array, int type) {
		cv::Mat _result;
		_createFromArray(L, array, type, _result);
		return _result;
	}

	cv::UMat createUMatFromArray(lua_State* L, const Table& array, int type, cv::UMatUsageFlags usageFlags) {
		cv::UMat _result(usageFlags);
		_createFromArray(L, array, type, _result);
		return _result;
	}

	int pushtable_Mat(lua_State* L, const cv::Mat& self, bool nested) {
		return _pushtable(L, self, nested);
	}

	int pushtable_UMat(lua_State* L, const cv::UMat& self, bool nested) {
		return _pushtable(L, self, nested);
	}
}
