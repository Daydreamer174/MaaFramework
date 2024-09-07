#include "TemplateComparator.h"

#include "Utils/Logger.h"
#include "Utils/NoWarningCV.hpp"
#include "VisionUtils.hpp"

MAA_VISION_NS_BEGIN

TemplateComparator::TemplateComparator(cv::Mat lhs, cv::Mat rhs, cv::Rect roi, TemplateComparatorParam param, std::string name)
    : VisionBase(std::move(lhs), std::move(roi), std::move(name))
    , rhs_image_(std::move(rhs))
    , param_(std::move(param))
{
    analyze();
}

void TemplateComparator::analyze()
{
    if (image_.size() != rhs_image_.size()) {
        LogError << "lhs_image_.size() != rhs_image_.size()" << VAR(image_) << VAR(rhs_image_);
        return;
    }

    auto start_time = std::chrono::steady_clock::now();

    cv::Mat lhs_roi = image_(correct_roi(roi_, image_));
    cv::Mat rhs_roi = rhs_image_(correct_roi(roi_, rhs_image_));
    double score = comp(lhs_roi, rhs_roi, param_.method);
    Result res = Result { .box = roi_, .score = score };

    add_results({ std::move(res) }, param_.threshold);

    cherry_pick();
    auto cost = duration_since(start_time);

    LogTrace << name_ << VAR(uid_) << VAR(all_results_) << VAR(filtered_results_) << VAR(best_result_) << VAR(cost);
}

void TemplateComparator::add_results(ResultsVec results, double threshold)
{
    std::ranges::copy_if(results, std::back_inserter(filtered_results_), [&](const auto& res) { return res.score > threshold; });

    merge_vector_(all_results_, std::move(results));
}

void TemplateComparator::cherry_pick()
{
    sort_by_score_(all_results_);
    sort_by_score_(filtered_results_);

    if (!filtered_results_.empty()) {
        best_result_ = filtered_results_.front();
    }
}

double TemplateComparator::comp(const cv::Mat& lhs, const cv::Mat& rhs, int method)
{
    cv::Mat matched;
    cv::matchTemplate(lhs, rhs, matched, method);

    double min_val = 0.0, max_val = 0.0;
    cv::Point min_loc {}, max_loc {};
    cv::minMaxLoc(matched, &min_val, &max_val, &min_loc, &max_loc);

    if (std::isnan(max_val) || std::isinf(max_val)) {
        max_val = 0;
    }

    return max_val;
}

MAA_VISION_NS_END
