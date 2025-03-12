
--- Module which builds the index.html page to be used in rocks servers.
local index = {}

local util = require("luarocks.util")
local fs = require("luarocks.fs")
local vers = require("luarocks.core.vers")
local persist = require("luarocks.persist")
local dir = require("luarocks.dir")
local manif = require("luarocks.manif")

local ext_url_target = ' target="_blank"'

local index_header = [[
<!DOCTYPE HTML>
<html lang="en">
   <head>
      <meta charset="utf-8"/>
      <title>Available rocks - LuaRocks</title>
      <link type="text/css" href="https://fonts.googleapis.com/css?family=Open+Sans:400italic,400,700" rel="stylesheet"/>
      <style>
         :root {
            --lua-blue: #00257A;
            --white-back-color: white;
            --back-color: #eff4ff;
            --header-color: #2c3e67;
            --header-fg: white;
            --button-color: #008FEE;
            --button-fg: white;
            --admin-color: #FFCAD9;
            --site-width: 960px;
            --link-color: #008FEE;
            --link-color-hover: #22a7ff;
            --button-height: 35px;
            --red-button-color: #EE004F;
            --base-font-size: 16px;
            --small-font-size: 14px;
            --body-color: #47555C;
            --border-color: #C1CCE4;
            --light-border-color: #d3dbec;
            --line-height: 24px;
            --input-border-color: #9dafd5;
            --input-border-color-focused: #4a68a9;
            --font-family: Open Sans, sans-serif;
            --sub-color: #757779;
            --sub-color-light: #919294;
            --error-color: #fa494d;
            --tag-color: #4b69af;
         }

         body {
            color: var(--body-color);
            font-size: var(--base-font-size);
            font-family: var(--font-family);
            margin: 0;
         }

         .header {
            background: var(--header-color);
            height: 55px;
            color: var(--header-fg);
         }

         .header .header_inner {
            max-width: var(--site-width);
            min-width: 800px;
            margin: 0 auto;
            height: 100%;
            display: flex;
            align-items: center;
            gap: 15px;
         }

         .content {
            background: var(--back-color);
            border-bottom: 1px solid var(--light-border-color);
         }

         .main_column {
            max-width: var(--site-width);
            min-width: 800px;
            margin: 0 auto;
            padding-bottom: 40px;
         }

         .footer {
            color: #9E9E9E;
            width: var(--site-width);
            margin: 0 auto;
            padding: 20px 0;
            font-size: 12px;
            display: flex;
         }

         .footer a {
            color: #9E9E9E;
         }

         .footer> :last-child {
            margin-left: auto;
         }

         .sub_footer {
            text-align: center;
            margin-bottom: 40px;
            margin-top: 40px;
         }

         .sub_footer span {
            display: block;
            color: #b4b4b4;
            font-size: 0.7em;
            font-weight: bold;
            text-transform: uppercase;
         }

         .sub_footer img {
            margin-top: 5px;
            opacity: 0.8;
            filter: grayscale(100%);
            transition: all 0.2s ease;
         }

         .sub_footer img:hover {
            opacity: 1;
            filter: grayscale(0%);
         }

         .module_list_widget {
            list-style: none;
            display: flex;
            flex-direction: column;
            gap: 10px;
            margin: 0;
            padding: 0;
         }

         .module_list_widget .module_row {
            background: var(--white-back-color);
            padding: 5px 10px;
            border-radius: 2px;
            border: 1px solid var(--border-color);
            line-height: 1.5;
            display: flex;
            flex-direction: column;
            gap: 5px;
         }

         .module_list_widget .module_row .title {
            font-size: 16px;
         }

         .module_list_widget .module_row .author {
            color: gray;
         }

         .module_list_widget .module_row .author a {
            color: #444;
         }

         .module_list_widget .module_row .author a:hover {
            color: #666;
         }

         .module_list_widget .module_row .main {
            display: flex;
            gap: 5px;
         }

         .module_list_widget .module_row .module_manifests {
            margin-left: auto;
            display: flex;
            gap: 5px;
         }

         .module_list_widget .module_row .module_manifests>a {
            text-decoration: none;
            margin: 0;
         }

         .module_list_widget .module_row .module_manifests>a:hover {
            color: var(--button-fg);
         }

         .module_list_widget .module_row .main,
         .module_list_widget .module_row .summary {
            white-space: nowrap;
            overflow: hidden;
            text-overflow: ellipsis;
         }

         .module_list_widget .module_row .summary {
            color: gray;
            font-size: var(--small-font-size);
         }

         .module_list_widget .module_row .downloads {
            color: gray;
         }

         .module_list_widget .module_row .downloads .value {
            color: #444;
         }

         .module_list_widget .module_row pre {
            font-family: inherit;
            font-size: 14px;
            overflow: auto;
         }

         .module_list_widget .module_row pre .term_snippet {
            background: #323744;
            color: white;
            font-size: 16px;
            border-radius: 2px;
            font-size: 14px;
            margin: 0;
            position: relative;
            display: block;
         }

         .module_list_widget .module_row pre .installer {
            padding: 16px 0px 16px 24px;
            display: block;
            font-size: 12px;
            overflow: auto;
         }

         .module_list_widget .module_row pre .nv {
            color: #ff9898;
         }

         .module_list_widget .module_row .versions {
            background-color: #d0d0d0;
            padding: 16px;
         }

         .module_list_widget .module_row pre .installer a {
            color: inherit;
         }
      </style>
   </head>
   <body>
      <main class="content">
         <header class="header">
            <div class="header_inner">
               <a href="/"><img src="https://luarocks.org/static/header_luarocks_name.svg" alt="LuaRocks" class="text_logo"/></a>
            </div>
         </header>
         <main class="search_page base_page base_widget">
            <div class="main_column">
               <h2>Modules</h2>
               <ul class="module_list_widget base_widget">
]]

local index_package_begin = [[
                  <li class="module_row">
                     <div class="main">
                        <h2>
                           <a name="$anchor" href="#$anchor" class="title">
                              <strong>$package</strong>
                           </a>
                           -
                           <span class="summary">$summary</span>
                        </h2>
                     </div>
                     <pre>$detailed</pre>
                     $externaldependencies
                     <div class="versions">
                        <h4>Versions</h4>
]]

local index_package_end = [[
                     </div>
                     <font size="-1"><a href="$original">latest sources</a> $homepage | License: $license</font>
                  </li>
]]

local index_footer_begin = [[
               </ul>
            </div>
         </main>
      </main>
      <div class="footer">
         <div class="left">
            <a href="manifest">manifest file</a>
]]
local index_manifest_ver = [[
&bull; <a href="manifest-$VER">Lua $VER manifest file</a>
]]
local index_footer_end = [[
         </div>
      </div>
   </body>
</html>
]]

local readme_header = [[
# ![LuaRocks](https://luarocks.org/static/header_luarocks_name.svg)

## Modules
]]

local readme_package_begin = [[
### $package

$summary

```
$detailed
```

#### Versions
]]

local readme_package_end = [[
[latest sources]($original) $homepage | License: $license
]]

local readme_footer_begin = '[manifest file](manifest)'
local readme_manifest_ver = ' . [Lua $VER manifest file](manifest-$VER)'

local function shallow_copy(t)
   local t2 = {}
   for k, v in pairs(t) do
      t2[k] = v
   end
   return t2
end

function index.format_external_dependencies(rockspec)
   if rockspec.external_dependencies then
      local deplist = {}
      local listed_set = {}
      local plats = nil
      for name, desc in util.sortedpairs(rockspec.external_dependencies) do
         if name ~= "platforms" then
            table.insert(deplist, name:lower())
            listed_set[name] = true
         else
            plats = desc
         end
      end
      if plats then
         for plat, entries in util.sortedpairs(plats) do
            for name, desc in util.sortedpairs(entries) do
               if not listed_set[name] then
                  table.insert(deplist, name:lower() .. " (on "..plat..")")
               end
            end
         end
      end
      return '<p><b>External dependencies:</b> ' .. table.concat(deplist, ',&nbsp;').. '</p>'
   else
      return ""
   end
end

function index.make_index(repo)
   if not fs.is_dir(repo) then
      return nil, "Cannot access repository at "..repo
   end
   local manifest = manif.load_manifest(repo)
   local out = io.open(dir.path(repo, "index.html"), "w")
   local readme = io.open(dir.path(repo, "README.md"), "w")

   out:write(index_header)
   readme:write(readme_header)
   for package, version_list in util.sortedpairs(manifest.repository) do
      local latest_rockspec = nil
      local output = index_package_begin
      local readme_output = '\n' .. readme_package_begin
      for version, data in util.sortedpairs(version_list, vers.compare_versions) do
         local versions = {}
         local readme_versions = {}
         output = output .. '<div class="version_row"><span class="version-name">' .. version ..':&nbsp;</span>'
         readme_output = readme_output .. '\n' .. version .. ': '
         table.sort(data, function(a,b) return a.arch < b.arch end)
         for _, item in ipairs(data) do
            local file
            if item.arch == 'rockspec' then
               file = ("%s-%s.rockspec"):format(package, version)
               if not latest_rockspec then latest_rockspec = file end
            else
               file = ("%s-%s.%s.rock"):format(package, version, item.arch)
            end
            table.insert(versions, '<a href="'..file..'">'..item.arch..'</a>')
            table.insert(readme_versions, '['..item.arch..'](' .. file .. ')')
         end
         output = output .. table.concat(versions, ',&nbsp;') .. '</div>'
         readme_output = readme_output .. table.concat(readme_versions, ', ')
      end
      output = output .. index_package_end
      readme_output = readme_output .. '\n\n' .. readme_package_end
      if latest_rockspec then
         local rockspec = persist.load_into_table(dir.path(repo, latest_rockspec))
         local descript = rockspec.description or {}
         local vars = {
            anchor = package,
            package = rockspec.package,
            original = rockspec.source.url,
            summary = descript.summary or "",
            detailed = descript.detailed or "",
            license = descript.license or "N/A",
            homepage = descript.homepage or "",
            externaldependencies = index.format_external_dependencies(rockspec)
         }

         local readme_vars = shallow_copy(vars)

         if vars.homepage then
            vars.homepage = '| <a href="'..descript.homepage..'"'..ext_url_target..'>project homepage</a>'
         end
         vars.detailed = vars.detailed:gsub("(luarocks install [^\r\n]+)\r?\n", '<span class="highlight lang_bash term_snippet"><code class="installer"><span class="nv">$ </span>%1</code></span>')
         vars.detailed = vars.detailed:gsub("(</code></span>[ \r\n]+<span class=\"highlight lang_bash term_snippet\"><code class=\"installer\">)", '\n')
         vars.detailed = vars.detailed:gsub("(https?://[a-zA-Z0-9%.%%-_%+%[%]=%?&/$@;:#]+)", '<a href="%1"'..ext_url_target..'>%1</a>')
         output = output:gsub("$(%w+)", vars)

         if readme_vars.homepage then
            readme_vars.homepage = '| [project homepage]('.. descript.homepage ..')'
         end
         readme_output = readme_output:gsub("$(%w+)", readme_vars)
      else
         output = output:gsub("$anchor", package)
         output = output:gsub("$package", package)
         output = output:gsub("$(%w+)", "")

         readme_output = readme_output:gsub("$anchor", package)
         readme_output = readme_output:gsub("$package", package)
         readme_output = readme_output:gsub("$(%w+)", "")
      end
      out:write(output)
      readme:write(readme_output)
   end
   out:write(index_footer_begin)
   readme:write('\n' .. readme_footer_begin)
   for ver in util.lua_versions() do
      out:write((index_manifest_ver:gsub("$VER", ver)))
      readme:write((readme_manifest_ver:gsub("$VER", ver)))
   end
   out:write(index_footer_end)
   out:close()
   readme:close()
end

return index
