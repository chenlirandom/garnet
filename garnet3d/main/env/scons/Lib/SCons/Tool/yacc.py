"""SCons.Tool.yacc

Tool-specific initialization for yacc.

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""

#
# Copyright (c) 2001, 2002, 2003, 2004 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "src\engine\SCons\Tool\yacc.py 0.96 2005/10/08 11:12:05 chenli"

import os.path

import SCons.Defaults
import SCons.Tool
import SCons.Util

YaccAction = SCons.Action.Action("$YACCCOM", "$YACCCOMSTR")

def _yaccEmitter(target, source, env, ysuf, hsuf):
    # If -d is specified on the command line, yacc will emit a .h
    # or .hpp file as well as a .c or .cpp file, depending on whether
    # the input file is a .y or .yy, respectively.
    if len(source) and '-d' in SCons.Util.CLVar(env.subst("$YACCFLAGS")):
        base, ext = os.path.splitext(SCons.Util.to_String(source[0]))
        if ext in ysuf:
            base, ext = os.path.splitext(SCons.Util.to_String(target[0]))
            target.append(base + env.subst(hsuf))
    return (target, source)

def yEmitter(target, source, env):
    return _yaccEmitter(target, source, env, ['.y', '.yacc'], '$YACCHFILESUFFIX')

def yyEmitter(target, source, env):
    return _yaccEmitter(target, source, env, ['.yy'], '$YACCHXXFILESUFFIX')

def generate(env):
    """Add Builders and construction variables for yacc to an Environment."""
    c_file, cxx_file = SCons.Tool.createCFileBuilders(env)
    
    c_file.add_action('.y', YaccAction)
    c_file.add_action('.yacc', YaccAction)
    cxx_file.add_action('.yy', YaccAction)
    c_file.add_emitter('.y', yEmitter)
    c_file.add_emitter('.yacc', yEmitter)
    cxx_file.add_emitter('.yy', yyEmitter)

    env['YACC']      = env.Detect('bison') or 'yacc'
    env['YACCFLAGS'] = SCons.Util.CLVar('')
    env['YACCCOM']   = '$YACC $YACCFLAGS -o $TARGET $SOURCES'
    env['YACCHFILESUFFIX'] = '.h'
    env['YACCHXXFILESUFFIX'] = '.hpp'

def exists(env):
    return env.Detect(['bison', 'yacc'])
