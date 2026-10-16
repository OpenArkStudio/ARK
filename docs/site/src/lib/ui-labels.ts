export type UiLocale = "en" | "zh-CN";

const english = {
  search: "Search",
  searchDocumentation: "Search documentation",
  searchPlaceholder: "Search documentation…",
  typeToSearch: "Type to search…",
  searching: "Searching…",
  noResults: "No results found.",
  searchBuildRequired: "Search is available after a production build.",
  searchLoadError: "Could not load search. Check your connection and reload the page.",
  searchUnavailable: "Search is temporarily unavailable.",
  closeSearch: "Close search",
  navigate: "navigate",
  select: "select",
  close: "close",
  onThisPage: "On this page",
  tableOfContents: "Table of contents",
  jumpToSection: "Jump to section",
  overview: "Overview",
  toggleDarkMode: "Toggle dark mode",
  filterPlaceholder: "Filter…",
  filterNavigation: "Filter navigation",
  pagination: "Pagination",
  previous: "Previous",
  next: "Next",
  openNavigation: "Open navigation",
  siteNavigation: "Site navigation",
  navigation: "Navigation",
  closeSidebar: "Close sidebar",
  editPage: "Edit this page"
};

const chinese: Record<keyof typeof english, string> = {
  search: "搜索",
  searchDocumentation: "搜索文档",
  searchPlaceholder: "搜索文档…",
  typeToSearch: "输入关键词开始搜索…",
  searching: "搜索中…",
  noResults: "未找到结果。",
  searchBuildRequired: "文档构建完成后可使用搜索。",
  searchLoadError: "无法加载搜索，请检查网络后重新加载页面。",
  searchUnavailable: "搜索暂时不可用。",
  closeSearch: "关闭搜索",
  navigate: "移动",
  select: "选择",
  close: "关闭",
  onThisPage: "本页内容",
  tableOfContents: "页内目录",
  jumpToSection: "跳转到章节",
  overview: "概览",
  toggleDarkMode: "切换深色模式",
  filterPlaceholder: "筛选…",
  filterNavigation: "筛选导航",
  pagination: "相邻页面",
  previous: "上一篇",
  next: "下一篇",
  openNavigation: "打开导航",
  siteNavigation: "站点导航",
  navigation: "导航",
  closeSidebar: "关闭侧边栏",
  editPage: "编辑此页"
};

export function getUiLabels(locale: UiLocale = "en") {
  return locale === "zh-CN" ? chinese : english;
}
