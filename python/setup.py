from distutils.core import setup, Extension
import lsb_release

print(lsb_release.get_distro_information())

if lsb_release.get_distro_information()['CODENAME'] == 'focal':
    python_lib = 'boost_python-py38'
elif lsb_release.get_distro_information()['CODENAME'] == 'xenial':
    python_lib = 'boost_python-py35'
else:
    python_lib = 'boost_python-py34'


module1 = Extension('bml',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['../sab_parser'],
                    #libraries = ['boost_python3'],
                    libraries = ['boost_python-py35'],
                    library_dirs = ['/usr/local/lib'],
                    sources = ['bml.cc','../sab_parser/bml.c'])

setup (name = 'bml',
       version = '1.0',
       description = 'This is the BML core binding library',
       author = 'Johann Baudy',
       author_email = 'johaahn@gnu-log.net',
       url = 'https://docs.python.org/extending/building',
       long_description = '''
This is really just a demo package.
''',
       ext_modules = [module1])

setup (name = 'bml_tools',
       version = '1.0',
       description = 'Bml tools package',
       author = 'Johann Baudy',
       author_email = 'johaahn@gnu-log.net',
       url = 'https://docs.python.org/extending/building',
       long_description = '''
This is really just a demo package.
''',
      py_modules=['bml_tools'])
