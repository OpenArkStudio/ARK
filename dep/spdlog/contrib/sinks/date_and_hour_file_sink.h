#pragma once

#include "../../details/file_helper.h"
#include "../../details/null_mutex.h"
#include "../../fmt/fmt.h"
#include "../../sinks/base_sink.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog
{
namespace sinks
{

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

/*
* Default generator of daily log file names.
*/
struct default_date_and_hour_file_name_calculator
{
    //
    // return file pre-dir and name and its extension:
    //
    // "mylog.txt" => ("", "mylog", ".txt")
    // "mylog" => ("", "mylog", "")
    // "mylog." => ("", "mylog.", "")
    // "/dir1/dir2/mylog.txt" => ("/dir1/dir2/", "mylog", ".txt")
    //
    // the starting dot in filenames is ignored (hidden files):
    //
    // ".mylog" => ("", ".mylog". "")
    // "my_folder/.mylog" => ("my_folder/", ".mylog", "")
    // "my_folder/.mylog.txt" => ("my_folder/", ".mylog", ".txt")
    static std::tuple<filename_t, filename_t, filename_t> split_by_dir_and_extenstion(const spdlog::filename_t& fname)
    {
        auto ext_index = fname.rfind('.');

        // no valid extension found - return whole path and empty string as extension
        if (ext_index == filename_t::npos || ext_index == 0 || ext_index == fname.size() - 1)
        {
            return std::make_tuple(spdlog::filename_t(), fname, spdlog::filename_t());
        }

        // treat cases like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
        auto folder_index = fname.rfind(details::os::folder_sep);

        if (folder_index != fname.npos && folder_index >= ext_index - 1)
        {
            return std::make_tuple(fname.substr(0, folder_index), fname.substr(folder_index + 1), spdlog::filename_t());
        }

        // finally - return a valid base and extension tuple
        return std::make_tuple(fname.substr(0, folder_index), fname.substr(folder_index + 1, ext_index - folder_index - 1), fname.substr(ext_index));
    }

    // Create filename for the form pre_dir/filename.YYYY-MM-DD_hh-mm.ext
    static filename_t calc_filename(const filename_t& filename)
    {
        std::tm tm = spdlog::details::os::localtime();
        filename_t pre_dir, basename, ext;
        std::tie(pre_dir, basename, ext) = split_by_dir_and_extenstion(filename);
        std::conditional<std::is_same<filename_t::value_type, char>::value, fmt::MemoryWriter, fmt::WMemoryWriter>::type w;
        auto dir_path = fmt::format(SPDLOG_FILENAME_T("{}{}{:04d}{:02d}{:02d}{}"), pre_dir, spdlog::details::os::folder_sep, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, spdlog::details::os::folder_sep);

        //CHECK if directory is already existed
        char tmp_dir_path[MAX_PATH] = { 0 };
        for (size_t i = 0; i < dir_path.length(); ++i)
        {
            tmp_dir_path[i] = dir_path[i];
            if (tmp_dir_path[i] == spdlog::details::os::folder_sep)
            {
                if (ACCESS(tmp_dir_path, 0) == -1)
                {
                    int32_t ret = MKDIR(tmp_dir_path);
                    if (ret != 0)
                    {
                        assert(0);
                    }
                }
            }
        }

        w.write(SPDLOG_FILENAME_T("{}{}_{:02d}{}"), dir_path, basename, tm.tm_hour, ext);
        return w.str();
    }
};

/*
* Rotating file sink based on date and hour. rotates at Hour:00:00
* Target path is Date/filename_hour.ext
*/
template<class Mutex, class FileNameCalc = default_date_and_hour_file_name_calculator>
class date_and_hour_file_sink SPDLOG_FINAL : public base_sink<Mutex>
{
public:
    // create daily file sink which rotates on given time
    date_and_hour_file_sink(filename_t base_filename)
        : _base_filename(std::move(base_filename))
    {
        _rotation_tp = _next_rotation_tp();
        _file_helper.open(FileNameCalc::calc_filename(_base_filename));
    }

protected:
    void _sink_it(const details::log_msg& msg) override
    {
        if (std::chrono::system_clock::now() >= _rotation_tp)
        {
            _file_helper.open(FileNameCalc::calc_filename(_base_filename));
            _rotation_tp = _next_rotation_tp();
        }

        _file_helper.write(msg);
    }

    void _flush() override
    {
        _file_helper.flush();
    }

private:
    std::chrono::system_clock::time_point _next_rotation_tp()
    {
        auto now = std::chrono::system_clock::now();
        now += std::chrono::hours(1);
        time_t tnow = std::chrono::system_clock::to_time_t(now);
        tm date = spdlog::details::os::localtime(tnow);
        date.tm_min = 0;
        date.tm_sec = 0;
        auto rotation_time = std::chrono::system_clock::from_time_t(std::mktime(&date));
        return rotation_time;
    }

    filename_t _base_filename;
    std::chrono::system_clock::time_point _rotation_tp;
    details::file_helper _file_helper;
};

using date_and_hour_file_sink_mt = date_and_hour_file_sink<std::mutex>;
using date_and_hour_file_sink_st = date_and_hour_file_sink<details::null_mutex>;

} // namespace sinks
} // namespace spdlog
