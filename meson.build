project('libadwaita', 'c',
        version : '0.1.dev',
        license : 'GPL-2.0',
        meson_version : '>= 0.59.0',
        default_options : ['buildtype=custom', 'c_std=gnu17'],
)

if get_option('buildtype') == 'custom'
    add_project_arguments('-Wall', '-Wextra', '-O0', '-ggdb', language : 'c')
endif

version_arr = meson.project_version().split('-')[0].split('.')
adwaita_version_major = version_arr[0].to_int()
adwaita_version_minor = version_arr[1].to_int()

if version_arr[2].startswith('dev') or version_arr[2].startswith('beta')
    adwaita_version_micro = 0
else
    adwaita_version_micro = version_arr[2].to_int()
endif

# The major api version as encoded in the libraries name
apiversion = adwaita_version_major.to_string()
package_api_name = '@0@-@1@'.format(meson.project_name(), apiversion)


gnome = import('gnome')

# Setup various paths that subdirectory meson.build files need
package_subdir = get_option('package_subdir') # When used as subproject
if package_subdir != ''
    datadir = get_option('datadir') / package_subdir
    libdir = get_option('libdir') / package_subdir
    girdir = get_option('datadir') / package_subdir / 'gir-1.0'
    typelibdir = get_option('libdir') / package_subdir / 'girepository-1.0'
    vapidir = get_option('datadir') / package_subdir / 'vapi'
else
    datadir = get_option('datadir')
    libdir = get_option('libdir')
    girdir = get_option('datadir') / 'gir-1.0'
    typelibdir = get_option('libdir') / 'girepository-1.0'
    vapidir = get_option('datadir') / 'vala' / 'vapi'
endif

subdir('po')
subdir('data')

demo_config_data = configuration_data()
demo_config_data.set_quoted('ADW_METAINFO', appstream_file.full_path())
demo_config_data.set_quoted('ADW_DEMO_VCS_TAG', '@VCS_TAG@')

demo_config_h = vcs_tag(
    input : configure_file(
        output : 'config.h.in',
        configuration : demo_config_data
    ),
    output : 'config.h'
)

adwaita_demo_resources = gnome.compile_resources(
    'ezp2023plus-resources',
    'ezp2023plus.gresources.xml',

    c_name : 'adw',
    dependencies : appstream_file,
    source_dir : meson.current_build_dir(),
)

adwaita_demo_sources = [
    adwaita_demo_resources,
    'windows/chips-editor/chips-editor.c',
    'windows/chips-editor/list-row.c',
    'main.c',
    'windows/main/main-window.c',
    'chips_data_repository.c',
    demo_config_h,
]

c = meson.get_compiler('c')
ezplib = meson.current_source_dir() + './lib/'
libezp2023plus_dep = c.find_library('ezp2023plus', dirs : ezplib)
libadwaita_dep = dependency('libadwaita-1', version : '>= 1.2')

adwaita_demo = executable('ezp2023plus'.format(apiversion),
                          adwaita_demo_sources,
                          dependencies : [libadwaita_dep, libezp2023plus_dep],
                          install : true,
                          include_directories : include_directories('include'),
)