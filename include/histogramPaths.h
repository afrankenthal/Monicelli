/*===============================================================================
 * Monicelli: the FERMILAB MTEST geometry builder and track reconstruction tool
 * 
 * Copyright (C) 2014 
 *
 * Authors:
 *
 * Dario Menasce      (INFN) 
 * Luigi Moroni       (INFN)
 * Jennifer Ngadiuba  (INFN)
 * Stefano Terzo      (INFN)
 * Lorenzo Uplegger   (FNAL)
 * Luigi Vigani       (INFN)
 *
 * INFN: Piazza della Scienza 3, Edificio U2, Milano, Italy 20126
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
 ================================================================================*/
 
#ifndef HISTOGRAMPATHS_H
#define HISTOGRAMPATHS_H

#define ALIGN_OUT_X_RES_ALL_SIZE       "Alignment_Results/A_Residuals/AR_X"
#define ALIGN_OUT_X_RES_SIZE_1         "Alignment_Results/A_Residuals/AR_X_1"
#define ALIGN_OUT_X_RES_SIZE_2         "Alignment_Results/A_Residuals/AR_X_2"
#define ALIGN_OUT_Y_RES_ALL_SIZE       "Alignment_Results/A_Residuals/AR_Y"
#define ALIGN_OUT_Y_RES_SIZE_1         "Alignment_Results/A_Residuals/AR_Y_1"
#define ALIGN_OUT_Y_RES_SIZE_2         "Alignment_Results/A_Residuals/AR_Y_2"

#define ALIGN_OUT_X_PULL_ALL_SIZE      "Alignment_Results/A_Pulls/AP_X"
#define ALIGN_OUT_X_PULL_SIZE_1        "Alignment_Results/A_Pulls/AP_X_1"
#define ALIGN_OUT_X_PULL_SIZE_2        "Alignment_Results/A_Pulls/AP_X_2"
#define ALIGN_OUT_Y_PULL_ALL_SIZE      "Alignment_Results/A_Pulls/AP_Y"
#define ALIGN_OUT_Y_PULL_SIZE_1        "Alignment_Results/A_Pulls/AP_Y_1"
#define ALIGN_OUT_Y_PULL_SIZE_2        "Alignment_Results/A_Pulls/AP_Y_2"

#define ALIGN_OUT_X_RES_Y_POS_ALL_SIZE "Alignment_Results/A_Correlation/AC_Xres_vs_Y/ACXY_Scatter_Plots"
#define ALIGN_OUT_X_RES_Y_POS_SIZE_1   "Alignment_Results/A_Correlation/AC_Xres_vs_Y/ACXY_1_Scatter_Plots"
#define ALIGN_OUT_X_RES_Y_POS_SIZE_2   "Alignment_Results/A_Correlation/AC_Xres_vs_Y/ACXY_2_Scatter_Plots"

#define ALIGN_OUT_Y_RES_X_POS_ALL_SIZE "Alignment_Results/A_Correlation/AC_Yres_vs_X/ACYX_Scatter_Plots"
#define ALIGN_OUT_Y_RES_X_POS_SIZE_1   "Alignment_Results/A_Correlation/AC_Yres_vs_X/ACYX_1_Scatter_Plots"
#define ALIGN_OUT_Y_RES_X_POS_SIZE_2   "Alignment_Results/A_Correlation/AC_Yres_vs_X/ACYX_2_Scatter_Plots"

#define ALIGN_OUT_X_RES_X_POS_ALL_SIZE "Alignment_Results/A_Correlation/AC_Xres_vs_X/ACXX_Scatter_Plots"
#define ALIGN_OUT_X_RES_X_POS_SIZE_1   "Alignment_Results/A_Correlation/AC_Xres_vs_X/ACXX_1_Scatter_Plots"
#define ALIGN_OUT_X_RES_X_POS_SIZE_2   "Alignment_Results/A_Correlation/AC_Xres_vs_X/ACXX_2_Scatter_Plots"

#define ALIGN_OUT_Y_RES_Y_POS_ALL_SIZE "Alignment_Results/A_Correlation/AC_Yres_vs_Y/ACYY_Scatter_Plots"
#define ALIGN_OUT_Y_RES_Y_POS_SIZE_1   "Alignment_Results/A_Correlation/AC_Yres_vs_Y/ACYY_1_Scatter_Plots"
#define ALIGN_OUT_Y_RES_Y_POS_SIZE_2   "Alignment_Results/A_Correlation/AC_Yres_vs_Y/ACYY_2_Scatter_Plots"

#define BEAM_SPOTS                     "Beam_Spots/B_2D"
#define BEAM_PROFILES_X                "Beam_Spots/B_Profiles/BP_X"
#define BEAM_PROFILES_Y                "Beam_Spots/B_Profiles/BP_Y"
#define DUT_PROJ                       "Beam_Spots/DUT_Projections"

#define CLUSTERS_OCCUPANCY             "Clusters/Cluster_Occupancy"
#define CLUSTERS_BEAM_PROFILES_X       "Clusters/C_Beam_Spots/CB_Profiles/CBP_X"
#define CLUSTERS_BEAM_PROFILES_Y       "Clusters/C_Beam_Spots/CB_Profiles/CBP_Y"
#define CLUSTERS_BEAM_SPOTS            "Clusters/C_Beam_Spots/CB_2D"

#define HITS_FREQUENCY                 "PulseHeight/Hits_Frequency"
#define ADC_DISTRIBUTIONS              "PulseHeight/ADC_Distributions"
#define FULL_CHARGE_DISTRIBUTIONS      "PulseHeight/S_Charge_Distributions"

#define X_U_RESIDUALS                  "Residuals/R_Unconstrained/UR_X"
#define Y_U_RESIDUALS                  "Residuals/R_Unconstrained/UR_Y"
#define X_C_RESIDUALS                  "Residuals/R_Constrained/CR_X"
#define Y_C_RESIDUALS                  "Residuals/R_Constrained/CR_Y"
#define X_RES_Y_POS_2D                 "Residuals/R_Correlation/RC_Xres_vs_Y/RCXY_Scatter_Plots"
#define Y_RES_X_POS_2D                 "Residuals/R_Correlation/RC_Yres_vs_X/RCYX_Scatter_Plots"
#define X_RES_X_POS_2D                 "Residuals/R_Correlation/RC_Xres_vs_X/RCXX_Scatter_Plots"
#define Y_RES_Y_POS_2D                 "Residuals/R_Correlation/RC_Yres_vs_Y/RCYY_Scatter_Plots"
#define X_RES_Y_POS_MEAN               "Residuals/R_Correlation/RC_Xres_vs_Y/RCXY_Plots"
#define Y_RES_X_POS_MEAN               "Residuals/R_Correlation/RC_Yres_vs_X/RCYX_Plots"
#define X_RES_X_POS_MEAN               "Residuals/R_Correlation/RC_Xres_vs_X/RCXX_Plots"
#define Y_RES_Y_POS_MEAN               "Residuals/R_Correlation/RC_Yres_vs_Y/RCYY_Plots"
#define X_PULLS                        "Residuals/R_Pulls/RP_X"
#define Y_PULLS                        "Residuals/R_Pulls/RP_Y"

#define TRACK_DISTRIBUTIONS            "Tracks"
#define X_TRACK_ERRORS_ON_DUT          "Tracks/Errors_on_DUT/TE_X"
#define Y_TRACK_ERRORS_ON_DUT          "Tracks/Errors_on_DUT/TE_Y"
#define TRACKS_INTERSECTION_BEAM_SPOTS "Tracks/T_Beam_Spots/Predicted_Points"
#define TRACKS_HITS_BEAM_SPOTS         "Tracks/T_Beam_Spots/Associated_Hits"
#define SELECTED_EV_CHARGE_DISTR       "Tracks/T_Charge_Distributions"

#define CALIBRATIONS                   "Calibrations"

#define KALMAN_FILTER_RES_X            "Kalman/FilteringX/FResidualsX"
#define KALMAN_FILTER_RES_Y            "Kalman/FilteringY/FResidualsY"
#define KALMAN_FILTER_PULLS_X          "Kalman/FilteringX/FPullsX"
#define KALMAN_FILTER_PULLS_Y          "Kalman/FilteringY/FPullsY"
#define KALMAN_FILTER_CHI2             "Kalman/Filtering/FChi2"
#define KALMAN_SMOOTH_RES_X            "Kalman/SmoothingX/SResidualsX"
#define KALMAN_SMOOTH_RES_Y            "Kalman/SmoothingY/SResidualsY"
#define KALMAN_SMOOTH_PULLS_X          "Kalman/SmoothingX/SPullsX"
#define KALMAN_SMOOTH_PULLS_Y          "Kalman/SmoothingY/SPullsY"
#define KALMAN_SMOOTH_CHI2             "Kalman/Smoothing/SChi2"

#endif // HISTOGRAMPATHS_H
