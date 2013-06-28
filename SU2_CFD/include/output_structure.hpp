/*!
 * \file output_structure.hpp
 * \brief Headers of the main subroutines for generating the file outputs.
 *        The subroutines and functions are in the <i>output_structure.cpp</i> file.
 * \author Aerospace Design Laboratory (Stanford University) <http://su2.stanford.edu>.
 * \version 2.0.2
 *
 * Stanford University Unstructured (SU2) Code
 * Copyright (C) 2012 Aerospace Design Laboratory
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <fstream>
#include <cmath>
#include <time.h>
#include <fstream>

#include "solution_structure.hpp"
#include "integration_structure.hpp"
#include "../../Common/include/geometry_structure.hpp"
#include "../../Common/include/config_structure.hpp"

#ifndef NO_CGNS
#include "cgnslib.h"
#endif

#ifndef NO_TECIO
#include "TECIO.h"
#endif

using namespace std;

/*! 
 * \class COutput
 * \brief Class for writing the flow, adjoint and linearized solver 
 *        solution (including the history solution, and parallel stuff).
 * \author F. Palacios, T. Economon, M. Colonno.
 * \version 2.0.2
 */
class COutput {

	unsigned long nGlobal_Poin;   // Global number of nodes with halos
  unsigned long nGlobal_Doma;   // Global number of nodes without halos
	unsigned long nGlobal_Elem,  // Global number of elems without halos
	nGlobal_Line,
	nGlobal_Tria,
	nGlobal_Quad,
	nGlobal_Tetr,
	nGlobal_Hexa,
	nGlobal_Wedg,
	nGlobal_Pyra;
	double **Coords;              // node i (x,y,z) = (Coords[0][i], Coords[1][i], Coords[2][i])
	unsigned long *Conn_Tria;	// triangle 1 = Conn_Tria[0], Conn_Tria[1], Conn_Tria[3]
	unsigned long *Conn_Quad;
	unsigned long *Conn_Tetr;
	unsigned long *Conn_Hexa;
	unsigned long *Conn_Wedg;
	unsigned long *Conn_Pyra;
	unsigned long *Conn_Line;
	double *Volume;
	double **Data;
	double **residuals, **consv_vars;					// placeholders
	double *p, *rho, *M, *Cp, *Cf, *Ch, *h, *yplus;		// placeholders 
	unsigned short nVar_Consv, nVar_Total, nZones; 
	bool wrote_CGNS_base, wrote_Tecplot_base;

protected:

public:

	/*! 
	 * \brief Constructor of the class. 
	 */
	COutput(void);

	/*! 
	 * \brief Destructor of the class. 
	 */
	~COutput(void);

	/*! 
	 * \brief Writes and organizes the all the output files, except the history one, for serial computations.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] iExtIter - Current external (time) iteration.
	 * \param[in] val_iZone - Total number of domains in the grid file.
   * \param[in] val_nZone - Total number of domains in the grid file.
	 */
	void SetResult_Files(CSolution ****solution_container, CGeometry ***geometry, CConfig **config, 
											 unsigned long iExtIter, unsigned short val_nZone);
	
	/*! 
	 * \brief Writes equivalent area.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetEquivalentArea(CSolution *solution_container, CGeometry *geometry, CConfig *config, 
			unsigned long iExtIter);
	
	/*!
	 * \brief Writes mass flow rate.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetFlowRate(CSolution *solution_container, CGeometry *geometry, CConfig *config,
                   unsigned long iExtIter);
  
	/*! 
	 * \brief Writes free surface, and functional.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetFreeSurface(CSolution *solution_container, CGeometry *geometry, CConfig *config, 
												 unsigned long iExtIter);

	/*! 
	 * \brief Create and write the file with the flow coefficient on the surface.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] FlowSolution - Flow solution.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetSurface_Flow(CConfig *config, CGeometry *geometry, CSolution *FlowSolution, unsigned long iExtIter);
	
	/*! 
	 * \brief Create and write the file with the adjoint coefficients on the surface for serial computations.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] AdjSolution - Adjoint solution.
	 * \param[in] FlowSolution - Flow solution.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetSurface_Adjoint(CConfig *config, CGeometry *geometry, CSolution *AdjSolution, CSolution *FlowSolution, unsigned long iExtIter);
	
	/*! 
	 * \brief Create and write the file with linearized coefficient on the surface for serial computations
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] LinSolution - Linearized solution.
	 * \param[in] val_filename - Name of the output file.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetSurface_Linearized(CConfig *config, CGeometry *geometry, CSolution *LinSolution, string val_filename, 
																			 unsigned long iExtIter);
	
	/*! 
	 * \brief Create and write the file with the flow coefficient on the surface.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] FlowSolution - Flow solution.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetSurfaceCSV_Flow(CConfig *config, CGeometry *geometry, CSolution *FlowSolution, unsigned long iExtIter);

	/*! 
	 * \brief Create and write the file with the adjoint coefficients on the surface for serial computations.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] AdjSolution - Adjoint solution.
	 * \param[in] FlowSolution - Flow solution.
	 * \param[in] iExtIter - Current external (time) iteration.
	 * \param[in] val_iZone - Current zone number in the grid file.
	 */
	void SetSurfaceCSV_Adjoint(CConfig *config, CGeometry *geometry, CSolution *AdjSolution, CSolution *FlowSolution, unsigned long iExtIter, unsigned short val_iZone);

	/*! 
	 * \brief Create and write the file with linearized coefficient on the surface for serial computations
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] LinSolution - Linearized solution.
	 * \param[in] val_filename - Name of the output file.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetSurfaceCSV_Linearized(CConfig *config, CGeometry *geometry, CSolution *LinSolution, string val_filename, unsigned long iExtIter);

  /*!
	 * \brief Merge the geometry into a data structure used for output file writing.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] val_nZone - iZone index.
	 */
	void MergeGeometry(CConfig *config, CGeometry *geometry, unsigned short val_iZone);
  
  /*!
	 * \brief Merge the node coordinates from all processors.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */
	void MergeCoordinates(CConfig *config, CGeometry *geometry);
  
  /*!
	 * \brief Merge the connectivity for a single element type from all processors.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] Elem_Type - VTK index of the element type being merged.
	 */
	void MergeConnectivity(CConfig *config, CGeometry *geometry, unsigned short Elem_Type);
  
	/*!
	 * \brief Merge the solution into a data structure used for output file writing.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] solution - Flow, adjoint or linearized solution.
	 * \param[in] val_nZone - iZone index.
	 */
	void MergeSolution(CConfig *config, CGeometry *geometry, CSolution **solution, unsigned short val_iZone);

  /*!
	 * \brief Write a native restart file.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] val_nZone - iZone index.
	 */
	void WriteRestart(CConfig *config, CGeometry *geometry, unsigned short val_iZone);

	/*! 
	 * \brief Write CGNS results file.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */
	void WriteCGNS(CConfig *config, CGeometry *geometry, unsigned short iZone);

  /*!
	 * \brief Write Tecplot ASCII results file.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
   * \param[in] val_iZone - Current zone.
   * \param[in] val_nZone - Total number of zones.
	 */
	void WriteTecplotASCII(CConfig *config, CGeometry *geometry, unsigned short val_iZone, unsigned short val_nZone);
  
	/*! 
	 * \brief Write Tecplot binary results file.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */
	void WriteTecplotBinary(CConfig *config, CGeometry *geometry, unsigned short iZone);

	/*! 
	 * \brief Deallocate temporary memory needed for solution merging and writing.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] geometry - Geometrical definition of the problem.
	 */
	void CleanUp(CConfig *config, CGeometry *geometry);

	/*! 
	 * \brief Write the header of the history file.
	 * \param[in] ConvHist_file - Pointer to the convergence history file (which is defined in the main subroutine).
	 * \param[in] config - Definition of the particular problem.
	 */
	void SetHistory_Header(ofstream *ConvHist_file, CConfig *config);

	/*! 
	 * \brief Write the history file and the convergence on the screen for serial computations.
	 * \param[in] ConvHist_file - Pointer to the convergence history file (which is defined in the main subroutine).
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] integration - Generic subroutines for space integration, time integration, and monitoring.
	 * \param[in] iExtIter - Current external (time) iteration.
	 * \param[in] timeused - Current number of clock tick in the computation (related with total time).
	 * \param[in] val_nZone - iZone index.
	 */
	void SetHistory_MainIter(ofstream *ConvHist_file, CGeometry ***geometry, CSolution ****solution_container, CConfig *config,
													 CIntegration ***integration, unsigned long iExtIter, unsigned long timeused, unsigned short val_iZone);

	/*! 
	 * \brief Write the history file and the convergence of the dual time algorithm for parallel computations.
	 * \param[in] geometry - Geometrical definition of the problem.
	 * \param[in] solution_container - Container vector with all the solutions.
	 * \param[in] config - Definition of the particular problem.
	 * \param[in] integration - Generic subroutines for space integration, time integration, and monitoring.
	 * \param[in] iExtIter - Current external (time) iteration.
	 */
	void SetHistory_DualTime(CGeometry ***geometry, CSolution ****solution_container, CConfig **config, CIntegration ***integration, unsigned long iExtIter, unsigned short val_iZone);

};