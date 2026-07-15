# Copyright 2015 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import glob
import os

from ament_copyright import ALL_FILETYPES
from ament_copyright import SOURCE_FILETYPE


def get_files(paths, extensions, exclude_patterns, skip_package_level_setup_py=True):
    excludes = []
    for exclude_pattern in exclude_patterns:
        excludes.extend(glob.glob(exclude_pattern))
    excludes = {os.path.realpath(x) for x in excludes}

    files = {}
    for path in paths:
        if os.path.isdir(path):
            if is_repository_root(path):
                add_files_for_all_filetypes(path, files)
            for dirpath, dirnames, filenames in os.walk(path):
                if 'AMENT_IGNORE' in dirnames + filenames:
                    dirnames[:] = []
                    continue
                if is_repository_root(dirpath):
                    add_files_for_all_filetypes(dirpath, files)

                # ignore folder starting with . or _
                dirnames[:] = [d for d in dirnames if d[0] not in ['.', '_']]
                dirnames.sort()

                # select files by extension
                for filename in sorted(filenames):
                    # skip package level setup.py file
                    if (
                        skip_package_level_setup_py and
                        filename == 'setup.py' and
                        os.path.exists(os.path.join(dirpath, 'package.xml'))
                    ):
                        continue
                    if match_filename(filename, extensions):
                        filepath = os.path.join(dirpath, filename)
                        if os.path.realpath(filepath) not in excludes:
                            files[os.path.join(dirpath, filename)] = SOURCE_FILETYPE

        if os.path.isfile(path) and match_filename(path, extensions):
            if os.path.realpath(path) not in excludes:
                files[path] = SOURCE_FILETYPE

        if is_repository_root(os.path.dirname(path)):
            basename = os.path.basename(path)
            for filetype, filename in ALL_FILETYPES.items():
                if filename == basename:
                    files[path] = filetype
                    break
    return {os.path.normpath(path): filetype for path, filetype in files.items()}


def is_repository_root(path):
    """Check if the path is the root of a git or mercurial repository."""
    return (
        os.path.exists(os.path.join(path, '.git')) or
        os.path.exists(os.path.join(path, '.hg'))
    )


def match_filename(filename, extensions):
    """Check if the filename has one of the extensions."""
    _, ext = os.path.splitext(filename)
    return ext in ('.%s' % e for e in extensions)


def add_files_for_all_filetypes(path, files):
    for filetype, filename in ALL_FILETYPES.items():
        if filename is None:
            continue
        files[os.path.join(path, filename)] = filetype
