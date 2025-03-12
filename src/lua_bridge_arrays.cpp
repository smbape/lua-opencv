#include <lua_bridge_arrays.hpp>

namespace LUA_MODULE_NAME {
	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>::_OptionalArray() {
		initialize();
	}

	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>::~_OptionalArray() {
		free();
	}

	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>::_OptionalArray(_OptionalArray<Array, is_arrays_type>& other) {
		*this = other;
	}

	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>::_OptionalArray(_OptionalArray<Array, is_arrays_type>&& other) noexcept {
		*this = std::move(other);
	}

	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>& _OptionalArray<Array, is_arrays_type>::operator=(_OptionalArray<Array, is_arrays_type>& other) {
		free();

		switch (other.m_index) {
			case 0:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					reset(*reinterpret_cast<bool*>(other.m_data));
				}
				break;
			case 1:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					reset(*reinterpret_cast<double*>(other.m_data));
				}
				break;
			case 2:
				if constexpr (!is_arrays_type) {
					if (other.m_is_shared_ptr) {
						reset(*reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(other.m_data));
					}
					reset(*reinterpret_cast<cv::cuda::GpuMat*>(other.m_data));
				}
				break;
			case 3:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<cv::Mat>*>(other.m_data));
				}
				reset(*reinterpret_cast<cv::Mat*>(other.m_data));
				break;
			case 4:
				if constexpr (!is_arrays_type) {
					if (other.m_is_shared_ptr) {
						reset(*reinterpret_cast<std::shared_ptr<cv::UMat>*>(other.m_data));
					}
					reset(*reinterpret_cast<cv::UMat*>(other.m_data));
				}
				break;
			case 5:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					reset(*reinterpret_cast<cv::Scalar*>(other.m_data));
				}
				break;
			case 6:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Mat>*>(other.m_data));
				break;
			case 7:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::UMat>*>(other.m_data));
				break;
			case 8:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					if (other.m_is_shared_ptr) {
						reset(*reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(other.m_data));
					}
					reset(*reinterpret_cast<std::vector<bool>*>(other.m_data));
				}
				break;
			case 9:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::RotatedRect>*>(other.m_data));
				break;
			case 10:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Range>*>(other.m_data));
				break;
			case 11:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Moments>*>(other.m_data));
				break;
			case 12:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<uchar>*>(other.m_data));
				break;
			case 13:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<schar>*>(other.m_data));
				break;
			case 14:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<char>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<char>*>(other.m_data));
				break;
			case 15:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<ushort>*>(other.m_data));
				break;
			case 16:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<short>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<short>*>(other.m_data));
				break;
			case 17:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<int>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<int>*>(other.m_data));
				break;
			case 18:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<float>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<float>*>(other.m_data));
				break;
			case 19:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<double>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<double>*>(other.m_data));
				break;
			case 20:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point3i>*>(other.m_data));
				break;
			case 21:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point3f>*>(other.m_data));
				break;
			case 22:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point3d>*>(other.m_data));
				break;
			case 23:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point2f>*>(other.m_data));
				break;
			case 24:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point2d>*>(other.m_data));
				break;
			case 25:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Point>*>(other.m_data));
				break;
			case 26:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Rect2f>*>(other.m_data));
				break;
			case 27:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Rect2d>*>(other.m_data));
				break;
			case 28:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Rect>*>(other.m_data));
				break;
			case 29:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Size2f>*>(other.m_data));
				break;
			case 30:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Size2d>*>(other.m_data));
				break;
			case 31:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Size>*>(other.m_data));
				break;
			case 32:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2b>*>(other.m_data));
				break;
			case 33:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3b>*>(other.m_data));
				break;
			case 34:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4b>*>(other.m_data));
				break;
			case 35:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2s>*>(other.m_data));
				break;
			case 36:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3s>*>(other.m_data));
				break;
			case 37:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4s>*>(other.m_data));
				break;
			case 38:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2w>*>(other.m_data));
				break;
			case 39:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3w>*>(other.m_data));
				break;
			case 40:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4w>*>(other.m_data));
				break;
			case 41:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2i>*>(other.m_data));
				break;
			case 42:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3i>*>(other.m_data));
				break;
			case 43:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4i>*>(other.m_data));
				break;
			case 44:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec6i>*>(other.m_data));
				break;
			case 45:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec8i>*>(other.m_data));
				break;
			case 46:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2f>*>(other.m_data));
				break;
			case 47:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3f>*>(other.m_data));
				break;
			case 48:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4f>*>(other.m_data));
				break;
			case 49:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec6f>*>(other.m_data));
				break;
			case 50:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec2d>*>(other.m_data));
				break;
			case 51:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec3d>*>(other.m_data));
				break;
			case 52:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec4d>*>(other.m_data));
				break;
			case 53:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<cv::Vec6d>*>(other.m_data));
				break;
			case 54:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					if (other.m_is_shared_ptr) {
						reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(other.m_data));
					}
					reset(*reinterpret_cast<std::vector<std::vector<bool>>*>(other.m_data));
				}
				break;
			case 55:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::RotatedRect>>*>(other.m_data));
				break;
			case 56:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Range>>*>(other.m_data));
				break;
			case 57:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Moments>>*>(other.m_data));
				break;
			case 58:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<uchar>>*>(other.m_data));
				break;
			case 59:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<schar>>*>(other.m_data));
				break;
			case 60:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<char>>*>(other.m_data));
				break;
			case 61:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<ushort>>*>(other.m_data));
				break;
			case 62:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<short>>*>(other.m_data));
				break;
			case 63:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<int>>*>(other.m_data));
				break;
			case 64:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<float>>*>(other.m_data));
				break;
			case 65:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<double>>*>(other.m_data));
				break;
			case 66:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point3i>>*>(other.m_data));
				break;
			case 67:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point3f>>*>(other.m_data));
				break;
			case 68:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point3d>>*>(other.m_data));
				break;
			case 69:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point2f>>*>(other.m_data));
				break;
			case 70:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point2d>>*>(other.m_data));
				break;
			case 71:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Point>>*>(other.m_data));
				break;
			case 72:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Rect2f>>*>(other.m_data));
				break;
			case 73:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Rect2d>>*>(other.m_data));
				break;
			case 74:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Rect>>*>(other.m_data));
				break;
			case 75:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Size2f>>*>(other.m_data));
				break;
			case 76:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Size2d>>*>(other.m_data));
				break;
			case 77:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Size>>*>(other.m_data));
				break;
			case 78:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2b>>*>(other.m_data));
				break;
			case 79:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3b>>*>(other.m_data));
				break;
			case 80:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4b>>*>(other.m_data));
				break;
			case 81:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2s>>*>(other.m_data));
				break;
			case 82:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3s>>*>(other.m_data));
				break;
			case 83:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4s>>*>(other.m_data));
				break;
			case 84:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2w>>*>(other.m_data));
				break;
			case 85:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3w>>*>(other.m_data));
				break;
			case 86:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4w>>*>(other.m_data));
				break;
			case 87:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2i>>*>(other.m_data));
				break;
			case 88:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3i>>*>(other.m_data));
				break;
			case 89:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4i>>*>(other.m_data));
				break;
			case 90:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec6i>>*>(other.m_data));
				break;
			case 91:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec8i>>*>(other.m_data));
				break;
			case 92:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2f>>*>(other.m_data));
				break;
			case 93:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3f>>*>(other.m_data));
				break;
			case 94:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4f>>*>(other.m_data));
				break;
			case 95:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec6f>>*>(other.m_data));
				break;
			case 96:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec2d>>*>(other.m_data));
				break;
			case 97:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec3d>>*>(other.m_data));
				break;
			case 98:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec4d>>*>(other.m_data));
				break;
			case 99:
				if (other.m_is_shared_ptr) {
					reset(*reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(other.m_data));
				}
				reset(*reinterpret_cast<std::vector<std::vector<cv::Vec6d>>*>(other.m_data));
				break;
			default:
				// Nothing to do
				break;
		}

		return *this;
	}

	// https://stackoverflow.com/questions/6687388/why-do-some-people-use-swap-for-move-assignments#answer-6687520
	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>& _OptionalArray<Array, is_arrays_type>::operator=(_OptionalArray<Array, is_arrays_type>&& other) noexcept {
		// 1. Destroy visible resources
		free();

		// 2. Move assign all bases and members.
		m_index = other.m_index;
		m_data = other.m_data;
		m_is_shared_ptr = other.m_is_shared_ptr;
		m_ptr = std::move(other.m_ptr);

		// 3. If the move assignment of bases and members didn't,
		//		   make the rhs resource-less, then make it so.
		other.m_index = -1;
		other.m_data = nullptr;
		other.m_is_shared_ptr = false;

		return *this;
	}

	template<typename Array, bool is_arrays_type>
	_OptionalArray<Array, is_arrays_type>::_OptionalArray(lua_State* L, int index, bool& is_valid, const bool nd_mat) {
		if (lua_isnil(L, index)) {
			is_valid = true;
			initialize();
			return;
		}

		if constexpr (is_arrays_type) {
			if (nd_mat) {
				auto value = lua_to(L, index, static_cast<cv::Mat*>(nullptr), is_valid);
				if (is_valid) {
					std::vector<cv::Mat> vec(1);
					vec.front() = extract_holder(value, static_cast<cv::Mat*>(nullptr));
					reset(std::move(vec));
					return;
				}
			}
		}

		if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
			{
				auto value = lua_to(L, index, static_cast<bool*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
			{
				auto value = lua_to(L, index, static_cast<double*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		if constexpr (!is_arrays_type) {
			{
				auto value = lua_to(L, index, static_cast<cv::cuda::GpuMat*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		{
			auto value = lua_to(L, index, static_cast<cv::Mat*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		if constexpr (!is_arrays_type) {
			{
				auto value = lua_to(L, index, static_cast<cv::UMat*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
			{
				auto value = lua_to(L, index, static_cast<cv::Scalar*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Mat>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::UMat>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		if constexpr (std::is_same_v<Array, cv::_InputArray>) {
			{
				auto value = lua_to(L, index, static_cast<std::vector<bool>*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::RotatedRect>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Moments>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<uchar>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<schar>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<char>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<ushort>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<short>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<int>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<float>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<double>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point3i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point3f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point3d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point2f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point2d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Point>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Rect2f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Rect2d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Rect>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Size2f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Size2d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Size>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2b>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3b>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4b>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2s>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3s>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4s>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2w>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3w>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4w>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec8i>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6f>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec2d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec3d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec4d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<cv::Vec6d>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
			{
				auto value = lua_to(L, index, static_cast<std::vector<std::vector<bool>>*>(nullptr), is_valid);
				if (is_valid) {
					reset(value);
					return;
				}
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::RotatedRect>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Range>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Moments>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<uchar>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<schar>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<char>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<ushort>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<short>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<int>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<float>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<double>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point3d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point2f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point2d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Point>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect2f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect2d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Rect>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size2f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size2d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Size>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2b>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3b>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4b>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2s>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3s>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4s>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2w>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3w>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4w>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec8i>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6f>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec2d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec3d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec4d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::vector<std::vector<cv::Vec6d>>*>(nullptr), is_valid);
			if (is_valid) {
				reset(value);
				return;
			}
		}

		is_valid = false;
	}

	template<typename Array, bool is_arrays_type>
	void _OptionalArray<Array, is_arrays_type>::initialize() {
		if constexpr (is_arrays_type) {
			reset(std::vector<cv::Mat>());
		}
		else {
			reset(cv::Mat());
		}
	}

	template<typename Array, bool is_arrays_type>
	void _OptionalArray<Array, is_arrays_type>::free() {
		if (m_ptr) {
			m_ptr.reset();
		}

		switch (m_index) {
			case 0:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					delete reinterpret_cast<bool*>(m_data);
				}
				break;
			case 1:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					delete reinterpret_cast<double*>(m_data);
				}
				break;
			case 2:
				if constexpr (!is_arrays_type) {
					if (m_is_shared_ptr) {
						delete reinterpret_cast<std::shared_ptr<cv::cuda::GpuMat>*>(m_data);
					} else {
						delete reinterpret_cast<cv::cuda::GpuMat*>(m_data);
					}
				}
				break;
			case 3:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<cv::Mat>*>(m_data);
				} else {
					delete reinterpret_cast<cv::Mat*>(m_data);
				}
				break;
			case 4:
				if constexpr (!is_arrays_type) {
					if (m_is_shared_ptr) {
						delete reinterpret_cast<std::shared_ptr<cv::UMat>*>(m_data);
					} else {
						delete reinterpret_cast<cv::UMat*>(m_data);
					}
				}
				break;
			case 5:
				if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray>) {
					delete reinterpret_cast<cv::Scalar*>(m_data);
				}
				break;
			case 6:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Mat>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Mat>*>(m_data);
				}
				break;
			case 7:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::UMat>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::UMat>*>(m_data);
				}
				break;
			case 8:
				if constexpr (std::is_same_v<Array, cv::_InputArray>) {
					if (m_is_shared_ptr) {
						delete reinterpret_cast<std::shared_ptr<std::vector<bool>>*>(m_data);
					} else {
						delete reinterpret_cast<std::vector<bool>*>(m_data);
					}
				}
				break;
			case 9:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::RotatedRect>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::RotatedRect>*>(m_data);
				}
				break;
			case 10:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Range>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Range>*>(m_data);
				}
				break;
			case 11:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Moments>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Moments>*>(m_data);
				}
				break;
			case 12:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<uchar>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<uchar>*>(m_data);
				}
				break;
			case 13:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<schar>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<schar>*>(m_data);
				}
				break;
			case 14:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<char>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<char>*>(m_data);
				}
				break;
			case 15:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<ushort>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<ushort>*>(m_data);
				}
				break;
			case 16:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<short>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<short>*>(m_data);
				}
				break;
			case 17:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<int>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<int>*>(m_data);
				}
				break;
			case 18:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<float>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<float>*>(m_data);
				}
				break;
			case 19:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<double>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<double>*>(m_data);
				}
				break;
			case 20:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point3i>*>(m_data);
				}
				break;
			case 21:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point3f>*>(m_data);
				}
				break;
			case 22:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point3d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point3d>*>(m_data);
				}
				break;
			case 23:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point2f>*>(m_data);
				}
				break;
			case 24:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point2d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point2d>*>(m_data);
				}
				break;
			case 25:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Point>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Point>*>(m_data);
				}
				break;
			case 26:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Rect2f>*>(m_data);
				}
				break;
			case 27:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect2d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Rect2d>*>(m_data);
				}
				break;
			case 28:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Rect>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Rect>*>(m_data);
				}
				break;
			case 29:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Size2f>*>(m_data);
				}
				break;
			case 30:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size2d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Size2d>*>(m_data);
				}
				break;
			case 31:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Size>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Size>*>(m_data);
				}
				break;
			case 32:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2b>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2b>*>(m_data);
				}
				break;
			case 33:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3b>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3b>*>(m_data);
				}
				break;
			case 34:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4b>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4b>*>(m_data);
				}
				break;
			case 35:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2s>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2s>*>(m_data);
				}
				break;
			case 36:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3s>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3s>*>(m_data);
				}
				break;
			case 37:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4s>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4s>*>(m_data);
				}
				break;
			case 38:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2w>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2w>*>(m_data);
				}
				break;
			case 39:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3w>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3w>*>(m_data);
				}
				break;
			case 40:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4w>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4w>*>(m_data);
				}
				break;
			case 41:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2i>*>(m_data);
				}
				break;
			case 42:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3i>*>(m_data);
				}
				break;
			case 43:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4i>*>(m_data);
				}
				break;
			case 44:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec6i>*>(m_data);
				}
				break;
			case 45:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec8i>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec8i>*>(m_data);
				}
				break;
			case 46:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2f>*>(m_data);
				}
				break;
			case 47:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3f>*>(m_data);
				}
				break;
			case 48:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4f>*>(m_data);
				}
				break;
			case 49:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6f>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec6f>*>(m_data);
				}
				break;
			case 50:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec2d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec2d>*>(m_data);
				}
				break;
			case 51:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec3d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec3d>*>(m_data);
				}
				break;
			case 52:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec4d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec4d>*>(m_data);
				}
				break;
			case 53:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<cv::Vec6d>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<cv::Vec6d>*>(m_data);
				}
				break;
			case 54:
				if constexpr (std::is_same_v<Array, cv::_InputOutputArray>) {
					if (m_is_shared_ptr) {
						delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<bool>>>*>(m_data);
					} else {
						delete reinterpret_cast<std::vector<std::vector<bool>>*>(m_data);
					}
				}
				break;
			case 55:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::RotatedRect>>*>(m_data);
				}
				break;
			case 56:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Range>>*>(m_data);
				}
				break;
			case 57:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Moments>>*>(m_data);
				}
				break;
			case 58:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<uchar>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<uchar>>*>(m_data);
				}
				break;
			case 59:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<schar>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<schar>>*>(m_data);
				}
				break;
			case 60:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<char>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<char>>*>(m_data);
				}
				break;
			case 61:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<ushort>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<ushort>>*>(m_data);
				}
				break;
			case 62:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<short>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<short>>*>(m_data);
				}
				break;
			case 63:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<int>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<int>>*>(m_data);
				}
				break;
			case 64:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<float>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<float>>*>(m_data);
				}
				break;
			case 65:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<double>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<double>>*>(m_data);
				}
				break;
			case 66:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point3i>>*>(m_data);
				}
				break;
			case 67:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point3f>>*>(m_data);
				}
				break;
			case 68:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point3d>>*>(m_data);
				}
				break;
			case 69:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point2f>>*>(m_data);
				}
				break;
			case 70:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point2d>>*>(m_data);
				}
				break;
			case 71:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Point>>*>(m_data);
				}
				break;
			case 72:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Rect2f>>*>(m_data);
				}
				break;
			case 73:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Rect2d>>*>(m_data);
				}
				break;
			case 74:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Rect>>*>(m_data);
				}
				break;
			case 75:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Size2f>>*>(m_data);
				}
				break;
			case 76:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Size2d>>*>(m_data);
				}
				break;
			case 77:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Size>>*>(m_data);
				}
				break;
			case 78:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2b>>*>(m_data);
				}
				break;
			case 79:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3b>>*>(m_data);
				}
				break;
			case 80:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4b>>*>(m_data);
				}
				break;
			case 81:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2s>>*>(m_data);
				}
				break;
			case 82:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3s>>*>(m_data);
				}
				break;
			case 83:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4s>>*>(m_data);
				}
				break;
			case 84:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2w>>*>(m_data);
				}
				break;
			case 85:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3w>>*>(m_data);
				}
				break;
			case 86:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4w>>*>(m_data);
				}
				break;
			case 87:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2i>>*>(m_data);
				}
				break;
			case 88:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3i>>*>(m_data);
				}
				break;
			case 89:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4i>>*>(m_data);
				}
				break;
			case 90:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec6i>>*>(m_data);
				}
				break;
			case 91:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec8i>>*>(m_data);
				}
				break;
			case 92:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2f>>*>(m_data);
				}
				break;
			case 93:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3f>>*>(m_data);
				}
				break;
			case 94:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4f>>*>(m_data);
				}
				break;
			case 95:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec6f>>*>(m_data);
				}
				break;
			case 96:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec2d>>*>(m_data);
				}
				break;
			case 97:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec3d>>*>(m_data);
				}
				break;
			case 98:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec4d>>*>(m_data);
				}
				break;
			case 99:
				if (m_is_shared_ptr) {
					delete reinterpret_cast<std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(m_data);
				} else {
					delete reinterpret_cast<std::vector<std::vector<cv::Vec6d>>*>(m_data);
				}
				break;
			default:
				// Nothind do do
				break;
		}

		m_index = -1;
		m_data = nullptr;
		m_is_shared_ptr = false;
	}

	template<typename Array, bool is_arrays_type>
	void _OptionalArray<Array, is_arrays_type>::reset() {
		free();
		initialize();
	}

	template<typename Array, bool is_arrays_type>
	template<typename T>
	void _OptionalArray<Array, is_arrays_type>::reset(T& value) {
		// reset(T& value);
		// m_data is a copy of value
		// m_ptr = std::make_shared<Array>(m_data);
		// m_index = TypeIndex
		// m_is_shared_ptr = false

		// reset(std::shared_ptr<T>& value);
		// m_data is a copy of shared_ptr
		// m_ptr = std::make_shared<Array>(*m_data);
		// m_index = TypeIndex
		// m_is_shared_ptr = true

		if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, bool>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 0;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, double>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 1;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, cv::cuda::GpuMat>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 2;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, std::shared_ptr<cv::cuda::GpuMat>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 2;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, cv::Mat>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 3;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<cv::Mat>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 3;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, cv::UMat>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 4;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, std::shared_ptr<cv::UMat>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 4;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, cv::Scalar>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 5;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Mat>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 6;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Mat>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 6;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::UMat>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 7;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::UMat>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 7;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, std::vector<bool>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 8;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, std::shared_ptr<std::vector<bool>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 8;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::RotatedRect>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 9;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::RotatedRect>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 9;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Range>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 10;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Range>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 10;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Moments>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 11;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Moments>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 11;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<uchar>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 12;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<uchar>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 12;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<schar>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 13;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<schar>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 13;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<char>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 14;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<char>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 14;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<ushort>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 15;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<ushort>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 15;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<short>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 16;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<short>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 16;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<int>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 17;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<int>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 17;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<float>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 18;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<float>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 18;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<double>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 19;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<double>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 19;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 20;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 20;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 21;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 21;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 22;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 22;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point2f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 23;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 23;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point2d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 24;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 24;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 25;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 25;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect2f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 26;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 26;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect2d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 27;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 27;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 28;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 28;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size2f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 29;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 29;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size2d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 30;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 30;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 31;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 31;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2b>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 32;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 32;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3b>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 33;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 33;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4b>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 34;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 34;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2s>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 35;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 35;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3s>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 36;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 36;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4s>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 37;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 37;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2w>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 38;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 38;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3w>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 39;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 39;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4w>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 40;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 40;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 41;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 41;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 42;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 42;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 43;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 43;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 44;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 44;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec8i>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 45;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec8i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 45;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 46;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 46;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 47;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 47;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 48;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 48;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6f>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 49;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 49;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 50;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 50;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 51;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 51;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 52;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 52;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6d>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 53;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 53;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputOutputArray> && std::is_same_v<T, std::vector<std::vector<bool>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 54;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputOutputArray> && std::is_same_v<T, std::shared_ptr<std::vector<std::vector<bool>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 54;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::RotatedRect>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 55;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 55;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Range>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 56;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Range>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 56;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Moments>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 57;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Moments>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 57;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<uchar>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 58;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<uchar>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 58;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<schar>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 59;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<schar>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 59;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<char>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 60;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<char>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 60;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<ushort>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 61;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<ushort>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 61;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<short>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 62;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<short>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 62;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<int>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 63;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<int>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 63;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<float>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 64;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<float>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 64;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 65;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<double>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 65;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 66;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 66;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 67;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 67;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 68;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 68;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 69;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point2f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 69;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 70;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point2d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 70;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 71;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 71;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 72;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 72;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 73;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 73;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 74;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 74;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 75;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size2f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 75;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 76;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size2d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 76;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 77;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 77;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 78;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 78;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 79;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 79;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4b>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 80;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 80;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 81;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 81;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 82;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 82;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4s>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 83;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 83;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 84;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 84;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 85;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 85;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4w>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 86;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 86;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 87;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 87;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 88;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 88;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 89;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 89;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 90;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 90;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec8i>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 91;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 91;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 92;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 92;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 93;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 93;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 94;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 94;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6f>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 95;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 95;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 96;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 96;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 97;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 97;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 98;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 98;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6d>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 99;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>>) {
			auto pp = new T(value);
			m_data = pp;
			m_index = 99;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else {
			static_assert(!std::is_same_v<T, T>, "Unsupported type");
		}
	}

	template<typename Array, bool is_arrays_type>
	template<typename T>
	void _OptionalArray<Array, is_arrays_type>::reset(T&& value) {
		// reset(T&& value);
		// m_data is a move of value
		// m_ptr = std::make_shared<Array>(m_data);
		// m_index = TypeIndex
		// m_is_shared_ptr = false

		// reset(std::shared_ptr<T>&& value);
		// m_data is a copy of shared_ptr
		// m_ptr = std::make_shared<Array>(*m_data);
		// m_index = TypeIndex
		// m_is_shared_ptr = true

		if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, bool>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 0;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, double>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 1;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, cv::cuda::GpuMat>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 2;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, std::shared_ptr<cv::cuda::GpuMat>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 2;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, cv::Mat>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 3;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<cv::Mat>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 3;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, cv::UMat>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 4;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<T, std::shared_ptr<cv::UMat>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 4;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (!is_arrays_type && std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, cv::Scalar>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 5;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Mat>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 6;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Mat>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 6;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::UMat>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 7;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::UMat>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 7;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, std::vector<bool>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 8;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputArray> && std::is_same_v<T, std::shared_ptr<std::vector<bool>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 8;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::RotatedRect>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 9;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::RotatedRect>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 9;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Range>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 10;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Range>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 10;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Moments>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 11;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Moments>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 11;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<uchar>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 12;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<uchar>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 12;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<schar>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 13;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<schar>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 13;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<char>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 14;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<char>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 14;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<ushort>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 15;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<ushort>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 15;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<short>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 16;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<short>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 16;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<int>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 17;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<int>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 17;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<float>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 18;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<float>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 18;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<double>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 19;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<double>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 19;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 20;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 20;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 21;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 21;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point3d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 22;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point3d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 22;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point2f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 23;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 23;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point2d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 24;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 24;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Point>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 25;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Point>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 25;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect2f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 26;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 26;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect2d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 27;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 27;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Rect>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 28;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Rect>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 28;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size2f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 29;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 29;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size2d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 30;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 30;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Size>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 31;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Size>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 31;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2b>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 32;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 32;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3b>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 33;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 33;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4b>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 34;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 34;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2s>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 35;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 35;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3s>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 36;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 36;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4s>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 37;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 37;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2w>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 38;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 38;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3w>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 39;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 39;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4w>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 40;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 40;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 41;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 41;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 42;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 42;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 43;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 43;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 44;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 44;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec8i>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 45;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec8i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 45;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 46;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 46;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 47;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 47;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 48;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 48;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6f>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 49;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 49;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec2d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 50;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 50;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec3d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 51;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec3d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 51;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec4d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 52;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec4d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 52;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<cv::Vec6d>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 53;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<cv::Vec6d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 53;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputOutputArray> && std::is_same_v<T, std::vector<std::vector<bool>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 54;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<Array, cv::_InputOutputArray> && std::is_same_v<T, std::shared_ptr<std::vector<std::vector<bool>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 54;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::RotatedRect>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 55;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 55;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Range>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 56;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Range>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 56;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Moments>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 57;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Moments>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 57;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<uchar>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 58;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<uchar>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 58;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<schar>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 59;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<schar>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 59;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<char>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 60;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<char>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 60;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<ushort>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 61;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<ushort>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 61;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<short>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 62;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<short>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 62;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<int>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 63;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<int>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 63;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<float>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 64;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<float>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 64;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 65;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<double>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 65;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 66;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 66;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 67;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 67;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point3d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 68;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point3d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 68;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 69;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point2f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 69;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 70;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point2d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 70;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Point>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 71;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Point>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 71;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 72;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 72;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 73;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 73;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Rect>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 74;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Rect>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 74;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 75;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size2f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 75;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 76;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size2d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 76;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Size>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 77;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Size>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 77;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 78;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 78;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 79;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 79;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4b>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 80;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 80;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 81;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 81;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 82;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 82;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4s>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 83;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 83;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 84;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 84;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 85;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 85;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4w>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 86;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 86;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 87;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 87;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 88;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 88;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 89;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 89;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 90;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 90;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec8i>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 91;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 91;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 92;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 92;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 93;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 93;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 94;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 94;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6f>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 95;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 95;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec2d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 96;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 96;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec3d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 97;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 97;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec4d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 98;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 98;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else if constexpr (std::is_same_v<T, std::vector<std::vector<cv::Vec6d>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 99;
			m_is_shared_ptr = false;
			m_ptr = std::make_shared<Array>(*pp);
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>>) {
			auto pp = new T(std::move(value));
			m_data = pp;
			m_index = 99;
			m_is_shared_ptr = true;
			m_ptr = std::make_shared<Array>(**pp);
		}
		else {
			static_assert(!std::is_same_v<T, T>, "Unsupported type");
		}
	}

	template<typename Array, bool is_arrays_type>
	int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array) {
		switch (opt_array.index()) {
			case 0:

				return lua_push(L, *reinterpret_cast<const bool*>(opt_array.data()));
			case 1:

				return lua_push(L, *reinterpret_cast<const double*>(opt_array.data()));
			case 2:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<cv::cuda::GpuMat>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const cv::cuda::GpuMat*>(opt_array.data()));
			case 3:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<cv::Mat>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const cv::Mat*>(opt_array.data()));
			case 4:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<cv::UMat>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const cv::UMat*>(opt_array.data()));
			case 5:

				return lua_push(L, *reinterpret_cast<const cv::Scalar*>(opt_array.data()));
			case 6:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Mat>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Mat>*>(opt_array.data()));
			case 7:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::UMat>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::UMat>*>(opt_array.data()));
			case 8:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<bool>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<bool>*>(opt_array.data()));
			case 9:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::RotatedRect>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::RotatedRect>*>(opt_array.data()));
			case 10:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Range>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Range>*>(opt_array.data()));
			case 11:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Moments>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Moments>*>(opt_array.data()));
			case 12:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<uchar>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<uchar>*>(opt_array.data()));
			case 13:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<schar>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<schar>*>(opt_array.data()));
			case 14:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<char>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<char>*>(opt_array.data()));
			case 15:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<ushort>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<ushort>*>(opt_array.data()));
			case 16:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<short>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<short>*>(opt_array.data()));
			case 17:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<int>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<int>*>(opt_array.data()));
			case 18:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<float>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<float>*>(opt_array.data()));
			case 19:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<double>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<double>*>(opt_array.data()));
			case 20:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point3i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point3i>*>(opt_array.data()));
			case 21:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point3f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point3f>*>(opt_array.data()));
			case 22:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point3d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point3d>*>(opt_array.data()));
			case 23:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point2f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point2f>*>(opt_array.data()));
			case 24:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point2d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point2d>*>(opt_array.data()));
			case 25:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Point>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Point>*>(opt_array.data()));
			case 26:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Rect2f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Rect2f>*>(opt_array.data()));
			case 27:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Rect2d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Rect2d>*>(opt_array.data()));
			case 28:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Rect>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Rect>*>(opt_array.data()));
			case 29:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Size2f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Size2f>*>(opt_array.data()));
			case 30:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Size2d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Size2d>*>(opt_array.data()));
			case 31:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Size>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Size>*>(opt_array.data()));
			case 32:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2b>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2b>*>(opt_array.data()));
			case 33:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3b>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3b>*>(opt_array.data()));
			case 34:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4b>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4b>*>(opt_array.data()));
			case 35:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2s>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2s>*>(opt_array.data()));
			case 36:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3s>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3s>*>(opt_array.data()));
			case 37:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4s>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4s>*>(opt_array.data()));
			case 38:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2w>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2w>*>(opt_array.data()));
			case 39:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3w>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3w>*>(opt_array.data()));
			case 40:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4w>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4w>*>(opt_array.data()));
			case 41:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2i>*>(opt_array.data()));
			case 42:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3i>*>(opt_array.data()));
			case 43:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4i>*>(opt_array.data()));
			case 44:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec6i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec6i>*>(opt_array.data()));
			case 45:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec8i>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec8i>*>(opt_array.data()));
			case 46:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2f>*>(opt_array.data()));
			case 47:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3f>*>(opt_array.data()));
			case 48:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4f>*>(opt_array.data()));
			case 49:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec6f>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec6f>*>(opt_array.data()));
			case 50:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec2d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec2d>*>(opt_array.data()));
			case 51:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec3d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec3d>*>(opt_array.data()));
			case 52:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec4d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec4d>*>(opt_array.data()));
			case 53:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<cv::Vec6d>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<cv::Vec6d>*>(opt_array.data()));
			case 54:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<bool>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<bool>>*>(opt_array.data()));
			case 55:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::RotatedRect>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::RotatedRect>>*>(opt_array.data()));
			case 56:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Range>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Range>>*>(opt_array.data()));
			case 57:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Moments>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Moments>>*>(opt_array.data()));
			case 58:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<uchar>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<uchar>>*>(opt_array.data()));
			case 59:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<schar>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<schar>>*>(opt_array.data()));
			case 60:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<char>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<char>>*>(opt_array.data()));
			case 61:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<ushort>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<ushort>>*>(opt_array.data()));
			case 62:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<short>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<short>>*>(opt_array.data()));
			case 63:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<int>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<int>>*>(opt_array.data()));
			case 64:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<float>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<float>>*>(opt_array.data()));
			case 65:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<double>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<double>>*>(opt_array.data()));
			case 66:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point3i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point3i>>*>(opt_array.data()));
			case 67:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point3f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point3f>>*>(opt_array.data()));
			case 68:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point3d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point3d>>*>(opt_array.data()));
			case 69:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point2f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point2f>>*>(opt_array.data()));
			case 70:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point2d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point2d>>*>(opt_array.data()));
			case 71:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Point>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Point>>*>(opt_array.data()));
			case 72:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Rect2f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Rect2f>>*>(opt_array.data()));
			case 73:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Rect2d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Rect2d>>*>(opt_array.data()));
			case 74:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Rect>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Rect>>*>(opt_array.data()));
			case 75:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Size2f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Size2f>>*>(opt_array.data()));
			case 76:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Size2d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Size2d>>*>(opt_array.data()));
			case 77:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Size>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Size>>*>(opt_array.data()));
			case 78:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2b>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2b>>*>(opt_array.data()));
			case 79:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3b>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3b>>*>(opt_array.data()));
			case 80:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4b>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4b>>*>(opt_array.data()));
			case 81:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2s>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2s>>*>(opt_array.data()));
			case 82:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3s>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3s>>*>(opt_array.data()));
			case 83:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4s>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4s>>*>(opt_array.data()));
			case 84:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2w>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2w>>*>(opt_array.data()));
			case 85:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3w>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3w>>*>(opt_array.data()));
			case 86:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4w>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4w>>*>(opt_array.data()));
			case 87:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2i>>*>(opt_array.data()));
			case 88:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3i>>*>(opt_array.data()));
			case 89:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4i>>*>(opt_array.data()));
			case 90:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec6i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec6i>>*>(opt_array.data()));
			case 91:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec8i>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec8i>>*>(opt_array.data()));
			case 92:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2f>>*>(opt_array.data()));
			case 93:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3f>>*>(opt_array.data()));
			case 94:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4f>>*>(opt_array.data()));
			case 95:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec6f>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec6f>>*>(opt_array.data()));
			case 96:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec2d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec2d>>*>(opt_array.data()));
			case 97:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec3d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec3d>>*>(opt_array.data()));
			case 98:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec4d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec4d>>*>(opt_array.data()));
			case 99:
				if (opt_array.is_shared_ptr()) {
					return lua_push(L, *reinterpret_cast<const std::shared_ptr<std::vector<std::vector<cv::Vec6d>>>*>(opt_array.data()));
				}
				return lua_push(L, *reinterpret_cast<const std::vector<std::vector<cv::Vec6d>>*>(opt_array.data()));
			default:
				lua_pushnil(L);
				return 1;
		}
	}

	// Template instantiation to force template definition in this file
	// since template declaration is in a header

	template class _OptionalArray<cv::_InputArray, false>;
	template class _OptionalArray<cv::_OutputArray, false>;
	template class _OptionalArray<cv::_InputOutputArray, false>;

	template class _OptionalArray<cv::_InputArray, true>;
	template class _OptionalArray<cv::_OutputArray, true>;
	template class _OptionalArray<cv::_InputOutputArray, true>;

	// Array parameter default value
	//
	// cv.omnidir.undistortImage void
	//	 InputArray distorted
	//	 OutputArray undistorted /O
	//	 InputArray K
	//	 InputArray D
	//	 InputArray xi
	//	 int flags
	//	 InputArray Knew cv::noArray()
	//	 Size new_size Size()/C; /Ref
	//	 InputArray R Mat::eye(3, 3, CV_64F)
	template void _OptionalArray<cv::_InputArray, false>::reset<cv::Mat>(cv::Mat&);

	template int lua_push<cv::_InputArray, false>(lua_State* L, const _OptionalArray<cv::_InputArray, false>& opt_array);
	template int lua_push<cv::_OutputArray, false>(lua_State* L, const _OptionalArray<cv::_OutputArray, false>& opt_array);
	template int lua_push<cv::_InputOutputArray, false>(lua_State* L, const _OptionalArray<cv::_InputOutputArray, false>& opt_array);

	template int lua_push<cv::_InputArray, true>(lua_State* L, const _OptionalArray<cv::_InputArray, true>& opt_array);
	template int lua_push<cv::_OutputArray, true>(lua_State* L, const _OptionalArray<cv::_OutputArray, true>& opt_array);
	template int lua_push<cv::_InputOutputArray, true>(lua_State* L, const _OptionalArray<cv::_InputOutputArray, true>& opt_array);
}
