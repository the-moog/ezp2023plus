project('epz2023plus', 'c',
        version : '0.1-dev',
        license : 'GPL-2.0-or-later',
        meson_version : '>= 0.59.0',
        default_options : ['buildtype=custom', 'c_std=gnu17'],
)

if get_option('buildtype') == 'custom'
    add_project_arguments('-Wall', '-Wextra', '-O0', '-ggdb', language : 'c')
endif

# Maybe will be used
#version_arr = meson.project_version().split('-')
#version_numbers = version_arr[0].split('.')
#app_version_major = version_numbers[0].to_int()
#app_version_minor = version_numbers[1].to_int()

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

app_config_data = configuration_data()
app_config_data.set_quoted('APP_METAINFO', appstream_file.full_path())
app_config_data.set_quoted('VCS_TAG', '@VCS_TAG@')

app_config_h = vcs_tag(
    input : configure_file(
        output : 'config.h.in',
        configuration : app_config_data
    ),
    output : 'config.h'
)

app_resources = gnome.compile_resources(
    'ezp2023plus-resources',
    'ezp2023plus.gresources.xml',

    c_name : 'adw',
    dependencies : appstream_file,
    source_dir : meson.current_build_dir(),
)

app_sources = [
    app_resources,
    'windows/chips-editor/chips-editor.c',
    'windows/chips-editor/list-row.c',
    'main.c',
    'windows/main/main-window.c',
    'chips_data_repository.c',
    'utilities.c',
    'gtk_string_list_extension.c',
    app_config_h,
]

cmake = import('cmake')
libezp_sub_proj = cmake.subproject('libezp2023plus')
libezp2023plus_dep = libezp_sub_proj.dependency('ezp2023plus')

libadwaita_dep = dependency('libadwaita-1', version : '>= 1.2')

app = executable('appezp2023plus',
                 app_sources,
                 dependencies : [libadwaita_dep, libezp2023plus_dep],
                 install : true,
)