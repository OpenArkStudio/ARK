# Business resources

The source tree keeps workbooks in `excel/`, generated metadata in `meta/`,
configuration rows and retained XML data in `server/`, and generated definitions
in `code/`. `client/` is an output placeholder; `mysql/` retains schema resources.
See [layout and runtime staging](../docs/reference/directory-structure.md#runtime-resources-and-generated-data)
and [configuration export](../docs/guides/tools.md); [中文说明](../docs/reference/directory-structure_CN.md#运行资源与生成数据).

Runtime applications still read a staged `resource/` directory. Copy only the
reviewed data needed by the selected application; generate its required metadata
before starting a Kernel-based service.

`photon/`, `script/` and `physic/` retain legacy protocol, Lua and physics assets.
The Photon parser binaries target x86-64 macOS/Linux and have no generator source
here; their compatibility has not been verified. The Lua/physics data and their
historical README files do not establish a working current integration. Business
XML, workbook and schema formats are preserved by the source-directory move.
