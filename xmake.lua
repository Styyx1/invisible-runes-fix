-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse")

-- set project
set_project("invisible-runes-fix")
set_version("1.0.0")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- set configs
set_config("skyrim_ae", true)

set_config("rex_ini", true)
add_extrafiles("release/**.ini")
-- targets
target("invisible-runes-fix")
    -- add dependencies to target
    add_deps("commonlibsse")

    -- change build directory based on the version to compile for
    if has_config("skyrim_ae") then
        set_targetdir("/build/SkyrimAE/skse/plugins")
    else
        set_targetdir("/build/SkyrimSE/skse/plugins")
    end  

    -- add commonlibsse plugin
    add_rules("commonlibsse.plugin", {
        name = "invisible-runes-fix",
        author = "styyx",
        description = "SKSE64 plugin template using CommonLibSSE"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

after_build(function(target)
    local copy = function(env, ext)
        for _, env in pairs(env:split(";")) do
            if os.exists(env) then
                local plugins = path.join(env, ext, "SKSE/Plugins")
                os.mkdir(plugins)
                os.trycp(target:targetfile(), plugins)
                os.trycp(target:symbolfile(), plugins)
                -- Copy config files or other extras
                os.trycp("$(projectdir)/release/**", plugins)
            end
        end
    end
    if os.getenv("XSE_TES5_MODS_PATH") then
        copy(os.getenv("XSE_TES5_MODS_PATH"), target:name())
    elseif os.getenv("XSE_TES5_GAME_PATH") then
        copy(os.getenv("XSE_TES5_GAME_PATH"), "Data")
    end    
end)
