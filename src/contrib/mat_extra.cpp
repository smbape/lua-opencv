#include <registration.hpp>

namespace {
	using namespace LUA_MODULE_NAME;
	using namespace cvextra;
	using namespace cv;
	using byte = unsigned char;

	template<typename... Args>
	inline int mat_at(lua_State* L, const cv::Mat& m, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			return lua_push(L, m.at<uchar>(std::forward<Args>(args)...));
		case CV_8S:
			return lua_push(L, m.at<char>(std::forward<Args>(args)...));
		case CV_16U:
			return lua_push(L, m.at<ushort>(std::forward<Args>(args)...));
		case CV_16S:
			return lua_push(L, m.at<short>(std::forward<Args>(args)...));
		case CV_32S:
			return lua_push(L, m.at<int>(std::forward<Args>(args)...));
		case CV_32F:
			return lua_push(L, m.at<float>(std::forward<Args>(args)...));
		case CV_64F:
			return lua_push(L, m.at<double>(std::forward<Args>(args)...));
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

	inline void setRange(std::vector<cv::Range>& ranges, const cv::Mat& self, const int i, const cv::Range& range, bool& expanded) {
		ranges[i] = range;

		if (ranges[i].start == INT_MIN) {
			expanded = true;
			ranges[i].start = 0;
		}

		if (ranges[i].end == INT_MAX) {
			expanded = true;
			const auto len = i < self.dims ? self.size.p[i] : self.channels();
			ranges[i].end = len;
		}
	}

	inline bool expand_ranges(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, int dims, std::vector<cv::Range>& ellipis_ranges) {
		const int ellipis_missing_ranges = dims - ranges.size();
		int ellipis_idx = -1;
		bool expanded = false;

		ellipis_ranges.resize(self.dims);

		for (int i = 0; i < ranges.size(); i++) {
			const auto& range = ranges[i];
			if (range.empty()) {
				LUAL_MODULE_ERROR(L, "range at index " << i << " must not be empty");
			}

			if (range == Ellipsis) {
				if (ellipis_idx != -1) {
					LUAL_MODULE_ERROR(L, "range at index " << i << " is another Ellipsis. There can only be at most one Ellipsis");
				}

				expanded = true;
				ellipis_ranges.resize(dims);
				ellipis_idx = i;

				for (int j = 0; j <= ellipis_missing_ranges; j++) {
					ellipis_ranges[j + ellipis_idx] = cv::Range::all();
				}
			}
			else if (ellipis_idx != -1) {
				setRange(ellipis_ranges, self, i + ellipis_missing_ranges, range, expanded);
			}
			else {
				setRange(ellipis_ranges, self, i, range, expanded);
			}
		}

		if (expanded && ellipis_idx == -1) {
			for (int i = ranges.size(); i < self.dims; i++) {
				ellipis_ranges[i] = cv::Range::all();
			}
		}

		return expanded;
	}

	inline void setRangeShape(std::vector<cv::Range>& ranges, std::vector<int>& newshape, const cv::Mat& self, const int i, const cv::Range& range) {
		bool expanded = false;
		setRange(ranges, self, i, range, expanded);
		const auto len = i < self.dims ? self.size.p[i] : self.channels();
		newshape.push_back(range == cv::Range::all() || range.size() >= len ? len : range.size());
	}

	inline void setRangeShape(
		lua_State* L,
		std::vector<cv::Range>& ranges,
		std::vector<int>& newshape,
		const cv::Mat& self,
		const std::vector<std::variant<int, cv::Range>>& idx,
		const int i,
		const int dims,
		int& ellipis_idx
	) {
		const int ellipis_missing_ranges = dims - idx.size();

		if (std::holds_alternative<cv::Range>(idx[i])) {
			const auto& range = std::get<cv::Range>(idx[i]);
			if (range.empty()) {
				LUAL_MODULE_ERROR(L, "range at index " << i << " is empty");
			}

			if (range == Ellipsis) {
				if (ellipis_idx != -1) {
					LUAL_MODULE_ERROR(L, "range at index " << i << " is another Ellipsis. There can only be at most one Ellipsis");
				}

				ranges.resize(dims);
				ellipis_idx = i;

				for (int j = 0; j <= ellipis_missing_ranges; j++) {
					setRangeShape(ranges, newshape, self, j + ellipis_idx, cv::Range::all());
				}
			}
			else if (ellipis_idx != -1) {
				setRangeShape(ranges, newshape, self, i + ellipis_missing_ranges, range);
			}
			else {
				setRangeShape(ranges, newshape, self, i, range);
			}
		}
		else if (ellipis_idx != -1) {
			const int k = i + ellipis_missing_ranges;
			const auto start = std::get<int>(idx[i]);
			ranges[k] = cv::Range(start, start + 1);
		}
		else {
			const auto start = std::get<int>(idx[i]);
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

	template<typename... Args>
	inline void setScalar(lua_State* L, cv::Mat& self, const cv::Mat& value, Args&&... args) {
		if (value.channels() * value.total() != 1) {
			LUAL_MODULE_ERROR(L, "value must be a scalar");
		}
		::mat_set_at(L, self, cvextra::mat_at(value, 0), std::forward<Args>(args)...);
	}

	template<typename... Args>
	inline void setScalar(lua_State* L, cv::Mat& self, const double value, Args&&... args) {
		::mat_set_at(L, self, value, std::forward<Args>(args)...);
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

	/**
	 * opencv/modules/core/src/precomp.hpp
	 * @param  sc     [description]
	 * @param  atype  [description]
	 * @param  sckind [description]
	 * @param  akind  [description]
	 * @return        [description]
	 */
	inline bool checkScalar(const Mat& sc, int atype, _InputArray::KindFlag sckind, _InputArray::KindFlag akind)
	{
		if (sc.dims > 2 || !sc.isContinuous())
			return false;
		Size sz = sc.size();
		if (sz.width != 1 && sz.height != 1)
			return false;
		int cn = CV_MAT_CN(atype);
		if (akind == _InputArray::MATX && sckind != _InputArray::MATX)
			return false;
		return sz == Size(1, 1) || sz == Size(1, cn) || sz == Size(cn, 1) ||
			(sz == Size(1, 4) && sc.type() == CV_64F && cn <= 4);
	}

	template<typename T>
	void setTo(cv::Mat& self, const T& value) {
		if constexpr (std::is_same_v<T, double>) {
			self.setTo(value);
		}
		else {
			if (checkScalar(value, self.type(), _InputArray::MAT, _InputArray::MAT)) {
				self.setTo(value);
			}
			else {
				value.copyTo(self);
			}
		}
	}

	template<typename T>
	void _Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const T& value) {
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

	template<typename T>
	void _Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const T& value) {
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
			_Mat_newindex_at(L, self, idx[0], value);
			return;
		}

		if (idx.size() == self.dims) {
			if (channels == 1) {
				setScalar(L, self, value, idx.data());
				return;
			}

			// only channels remains

			static std::vector<cv::Range> ranges; ranges.resize(idx.size());
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

		static std::vector<cv::Range> ranges; ranges.resize(self.dims);

		for (int i = 0; i < idx.size(); i++) {
			ranges[i] = cv::Range(idx[i], idx[i] + 1);
		}

		for (int i = idx.size(); i < self.dims; i++) {
			ranges[i] = cv::Range::all();
		}

		cv::Mat res(self, ranges);
		std::vector<int> newshape(size.p + idx.size(), size.p + size.dims());
		res = res.reshape(0, newshape);
		setTo(res, value);
	}

	template<typename T>
	void _Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& _ranges, const T& value) {
		if (_ranges.size() == 0) {
			LUAL_MODULE_ERROR(L, "ranges can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (_ranges.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than ranges " << _ranges.size() << " dimensions");
		}

		static std::vector<cv::Range> ellipis_ranges;
		const bool expanded = expand_ranges(L, self, _ranges, dims, ellipis_ranges);
		const auto& ranges = expanded ? ellipis_ranges : _ranges;

		if (ranges.size() == self.dims) {
			auto res = cv::Mat(self, ranges);
			setTo(res, value);
			return;
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

		ellipis_ranges.resize(self.dims);

		for (int i = 0; i < ranges.size(); i++) {
			ellipis_ranges[i] = ranges[i];
		}

		for (int i = ranges.size(); i < self.dims; i++) {
			ellipis_ranges[i] = cv::Range::all();
		}

		auto res = cv::Mat(self, ellipis_ranges);
		setTo(res, value);
	}

	template<typename T>
	void _Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const T& value) {
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

		if (dims == 1 && std::holds_alternative<int>(idx[0])) {
			_Mat_newindex_at(L, self, std::get<int>(idx[0]), value);
			return;
		}

		static std::vector<cv::Range> ranges; ranges.resize(idx.size());
		std::vector<int> newshape;
		int ellipis_idx = -1;
		for (int i = 0; i < idx.size(); i++) {
			setRangeShape(L, ranges, newshape, self, idx, i, dims, ellipis_idx);
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

		ranges.resize(self.dims);

		for (int i = idx.size(); i < self.dims; i++) {
			ranges[i] = cv::Range::all();
		}

		cv::Mat res(self, ranges);
		res = check_reshape(L, res, newshape, idx);
		setTo(res, value);
	}

}

namespace cvextra {
	cv::Range Ellipsis(INT_MAX, INT_MIN);

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

	int Mat_index_at(lua_State* L, cv::Mat& self, int idx) {
		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		if (dims == 1) {
			if (channels != 1) {
				// treated as dims == 2
				int newsz[] = { channels };
				return lua_push(L, self.row(idx).reshape(1, 1, newsz));
			}

			return ::mat_at(L, self, idx);
		}

		if (dims > 2) {
			cv::Mat res = self.row(idx);
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			return lua_push(L, res.reshape(channels, dims - 1, size.p + 1));
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			cv::Mat res = self.row(idx);
			int newsz[] = { size[1], channels };
			if (!res.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet.");
			}
			return lua_push(L, res.reshape(1, 2, newsz));
		}

		// dims == 2, channels == 1

		// opencv Mat does not support 1D matrix
		// treat a matrix with 1 channel and (1 row or 1 column) as a 1D matrix
		if (size[0] == 1 || size[1] == 1) {
			return ::mat_at(L, self, idx);
		}

		return lua_push(L, self.row(idx));
	}

	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx) {
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
			return lua_push(L, Mat_index_at(L, self, idx[0]));
		}

		if (idx.size() == self.dims) {
			if (channels == 1) {
				return ::mat_at(L, self, idx.data());
			}

			// only channels remains

			static std::vector<cv::Range> ranges; ranges.resize(idx.size());
			for (int i = 0; i < idx.size(); i++) {
				ranges[i] = cv::Range(idx[i], idx[i] + 1);
			}

			cv::Mat res(self, ranges);

			// channels are always continuous, therefore, reshaping to channels only is always possible
			int newsz[] = { channels };
			return lua_push(L, res.reshape(1, 1, newsz));
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

		static std::vector<cv::Range> ranges; ranges.resize(self.dims);

		for (int i = 0; i < idx.size(); i++) {
			ranges[i] = cv::Range(idx[i], idx[i] + 1);
		}

		for (int i = idx.size(); i < self.dims; i++) {
			ranges[i] = cv::Range::all();
		}

		cv::Mat res(self, ranges);
		std::vector<int> newshape(size.p + idx.size(), size.p + size.dims());
		return lua_push(L, res.reshape(0, newshape));
	}

	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& _ranges) {
		if (_ranges.size() == 0) {
			LUAL_MODULE_ERROR(L, "ranges can not be empty");
		}

		const auto& size = self.size;
		const auto channels = self.channels();

		auto dims = self.dims;
		if (channels != 1) {
			dims++;
		}

		if (_ranges.size() > dims) {
			LUAL_MODULE_ERROR(L, "matrix " << dims << " dimensions is less than ranges " << _ranges.size() << " dimensions");
		}

		static std::vector<cv::Range> ellipis_ranges;
		const bool expanded = expand_ranges(L, self, _ranges, dims, ellipis_ranges);
		const auto& ranges = expanded ? ellipis_ranges : _ranges;

		if (ranges.size() == self.dims) {
			return lua_push(L, std::make_shared<cv::Mat>(self, ranges));
		}

		if (ranges.size() == dims) {
			// channels is treated as a dimension

			if (!self.isContinuous()) {
				LUAL_MODULE_ERROR(L, "Reshaping of n-dimensional non-continuous matrices is not supported yet. Channels cannot be treated as a new dimension");
			}

			// reshape with channels added as a new dimension
			std::vector<int> newsz(size.p, size.p + size.dims());
			newsz.push_back(channels);
			return lua_push(L, std::make_shared<cv::Mat>(self.reshape(1, newsz), ranges));
		}

		// ranges.size() < self.dims

		ellipis_ranges.resize(self.dims);

		for (int i = 0; i < ranges.size(); i++) {
			ellipis_ranges[i] = ranges[i];
		}

		for (int i = ranges.size(); i < self.dims; i++) {
			ellipis_ranges[i] = cv::Range::all();
		}

		return lua_push(L, std::make_shared<cv::Mat>(self, ellipis_ranges));
	}

	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx) {
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

		if (dims == 1 && std::holds_alternative<int>(idx[0])) {
			return Mat_index_at(L, self, std::get<int>(idx[0]));
		}

		static std::vector<cv::Range> ranges; ranges.resize(idx.size());
		std::vector<int> newshape;
		int ellipis_idx = -1;
		for (int i = 0; i < idx.size(); i++) {
			setRangeShape(L, ranges, newshape, self, idx, i, dims, ellipis_idx);
		}

		if (idx.size() == self.dims) {
			// only channels remains
			cv::Mat res(self, ranges);
			return lua_push(L, check_reshape(L, res, newshape, idx));
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

			return lua_push(L, check_reshape(L, res, newshape, idx));
		}

		// idx.size() < self.dims

		ranges.resize(self.dims);

		for (int i = idx.size(); i < self.dims; i++) {
			ranges[i] = cv::Range::all();
		}

		for (int i = idx.size(); i < self.dims; i++) {
			ranges[i] = cv::Range::all();
		}

		cv::Mat res(self, ranges);
		return lua_push(L, check_reshape(L, res, newshape, idx));
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const cv::Mat& value) {
		_Mat_newindex_at(L, self, idx, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const double value) {
		_Mat_newindex_at(L, self, idx, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const cv::Mat& value) {
		_Mat_newindex_at(L, self, idx, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const double value) {
		_Mat_newindex_at(L, self, idx, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, const cv::Mat& value) {
		_Mat_newindex_at(L, self, ranges, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, const double value) {
		_Mat_newindex_at(L, self, ranges, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const cv::Mat& value) {
		_Mat_newindex_at(L, self, idx, value);
	}

	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const double value) {
		_Mat_newindex_at(L, self, idx, value);
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
