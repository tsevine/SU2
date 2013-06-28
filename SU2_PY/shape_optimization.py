#!/usr/bin/env python 

## \file shape_optimization.py
#  \brief Python script for performing the shape optimization.
#  \author Francisco Palacios, Tom Economon, Trent Lukaczyk, Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
#  \version 2.0.2
#
# Stanford University Unstructured (SU2) Code
# Copyright (C) 2012 Aerospace Design Laboratory
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os, sys, shutil, copy
from optparse import OptionParser
import SU2

# -------------------------------------------------------------------
#  Main 
# -------------------------------------------------------------------

def main():

    parser=OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="read config from FILE", metavar="FILE")
    parser.add_option("-n", "--name", dest="name", default='',
                      help="project file NAME", metavar="NAME")
    parser.add_option("-p", "--partitions", dest="partitions", default=1,
                      help="number of PARTITIONS", metavar="PARTITIONS")
    parser.add_option("-g", "--gradient", dest="gradient", default="Adjoint",
                      help="Method for computing the GRADIENT (Adjoint or FinDiff)", metavar="GRADIENT")
    parser.add_option("-q", "--quiet", dest="quiet", default="False",
                      help="True/False Quiet all SU2 output (optimizer output only)", metavar="QUIET")
    parser.add_option("-c", "--cycle", dest="cycle", default=0,
                      help="number of mesh adaptation CYCLEs", metavar="CYCLE")
    parser.add_option("-s", "--step", dest="step", default=1e-4,
                      help="finite difference STEP", metavar="STEP")
    
    (options, args)=parser.parse_args()
    
    # process inputs
    options.partitions  = int( options.partitions )
    options.cycle       = int( options.cycle )
    options.step        = float( options.step )
    options.quiet       = options.quiet.upper() == 'TRUE'
    options.gradient    = options.gradient.upper()
    
    shape_optimization( options.filename   ,
                        options.name       ,
                        options.partitions ,
                        options.gradient   ,
                        options.quiet      ,
                        options.cycle      ,
                        options.step        )
    
#: main()

def shape_optimization( filename                , 
                        projectname = ''        ,
                        partitions  = 0         , 
                        gradient    = 'ADJOINT' ,
                        quiet       = False     , 
                        cycle       = 0         ,
                        step        = 1e-4       ):
    # todo:
    # gradient method
    # step
    
    # Config
    config = SU2.io.Config(filename)
    config.NUMBER_PART = partitions
    if quiet: config.CONSOLE = 'QUIET'
    
    def_dv = config.DEFINITION_DV
    n_dv   = len(def_dv['KIND'])  
    x0     = [0.0]*n_dv # initial design
    xb     = []         # design bounds
    its    = 100        # max iterations
    
    # State
    state = SU2.io.State()
    state.find_files(config)
    
    # Project
    if os.path.exists(projectname):
        project = SU2.io.load_data(projectname)
    else:
        project = SU2.opt.Project(config,state)
    
    # Optimize
    SU2.opt.SLSQP(project,x0,xb,its)
    
    # rename project file
    if projectname:
        shutil.move('project.pkl',projectname)
    
    return project

#: shape_optimization()


# -------------------------------------------------------------------
#  Run Main Program
# -------------------------------------------------------------------

# this is only accessed if running from command prompt
if __name__ == '__main__':
    main()
