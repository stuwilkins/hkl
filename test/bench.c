/* This file is part of the hkl library.
 *
 * The hkl library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The hkl library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the hkl library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2003-2009 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#include <stdio.h>
#include <sys/time.h>
#include <hkl.h>

#define SET_AXES(geometry, mu, komega, kappa, kphi, gamma, delta) do{\
	hkl_geometry_set_values_v(geometry, 6,\
				  mu * HKL_DEGTORAD,\
				  komega * HKL_DEGTORAD,\
				  kappa * HKL_DEGTORAD,\
				  kphi * HKL_DEGTORAD,\
				  gamma * HKL_DEGTORAD,\
				  delta * HKL_DEGTORAD);\
} while(0)

static void hkl_test_bench_run(HklPseudoAxisEngine *engine, HklGeometry *geometry, size_t n)
{
	size_t i, j;
	struct timeval debut, fin, dt;

	// pseudo -> geometry
	for(j=0; j<HKL_LIST_LEN(engine->modes); ++j){
		hkl_pseudo_axis_engine_select_mode(engine, j);
		if (HKL_LIST_LEN(engine->mode->parameters))
			engine->mode->parameters[0].value = 1.;

		gettimeofday(&debut, NULL);
		for(i=0; i<n; ++i){
			SET_AXES(geometry, 0, 0, 0, 0, 10, 10);
			hkl_pseudo_axis_engine_setter(engine);
		}
		gettimeofday(&fin, NULL);
		timersub(&fin, &debut, &dt);
		fprintf(stdout, "%d %s (%d/%d) iterations %f ms each\n",
			j, engine->mode->name, n, i, (dt.tv_sec*1000.+dt.tv_usec/1000.)/n);
	}

}

static void hkl_test_bench_hkl_real(HklPseudoAxisEngineList *engines, HklGeometry *geometry,
				    char const *name, int n,
				    double h, double k, double l)
{
	HklPseudoAxisEngine *engine;

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, name);

	((HklParameter *)engine->pseudoAxes[0])->value = h;
	((HklParameter *)engine->pseudoAxes[1])->value = k;
	((HklParameter *)engine->pseudoAxes[2])->value = l;

	hkl_test_bench_run(engine, geometry, n);
}

static void hkl_test_bench_eulerians_real(HklPseudoAxisEngineList *engines, HklGeometry *geometry,
					  char const *name, int n,
					  double omega, double chi, double phi)
{
	HklPseudoAxisEngine *engine;

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, name);

	((HklParameter *)engine->pseudoAxes[0])->value = omega;
	((HklParameter *)engine->pseudoAxes[1])->value = chi;
	((HklParameter *)engine->pseudoAxes[2])->value = phi;

	hkl_test_bench_run(engine, geometry, n);
}

static void hkl_test_bench_psi_real(HklPseudoAxisEngineList *engines, HklGeometry *geometry,
				    char const *name, int n,
				    double psi)
{
	HklPseudoAxisEngine *engine;

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, name);

	((HklParameter *)engine->pseudoAxes[0])->value = psi;

	hkl_test_bench_run(engine, geometry, n);
}

static void hkl_test_bench_q2_real(HklPseudoAxisEngineList *engines, HklGeometry *geometry,
				   char const *name, int n,
				   double q, double alpha)
{
	HklPseudoAxisEngine *engine;

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, name);

	((HklParameter *)engine->pseudoAxes[0])->value = q;
	((HklParameter *)engine->pseudoAxes[1])->value = alpha;

	hkl_test_bench_run(engine, geometry, n);
}

static void hkl_test_bench_k6c()
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, j;
	int res, n;

	geom = hkl_geometry_factory_new(HKL_GEOMETRY_TYPE_KAPPA6C, 50 * HKL_DEGTORAD);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	sample = hkl_sample_new("test", HKL_SAMPLE_MONOCRYSTAL);

	engines = hkl_pseudo_axis_engine_list_factory(HKL_GEOMETRY_TYPE_KAPPA6C);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	hkl_test_bench_hkl_real(engines, geom, "hkl", 1000, 1, 0, 0 );
	hkl_test_bench_eulerians_real(engines, geom, "eulerians", 1000, 0, 90*HKL_DEGTORAD, 0 );
	hkl_test_bench_psi_real(engines, geom, "psi", 1000, 10*HKL_DEGTORAD);
	hkl_test_bench_q2_real(engines, geom, "q2", 1000, 1, 10*HKL_DEGTORAD);

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_sample_free(sample);
	hkl_detector_free(detector);
	hkl_geometry_free(geom);
}

hkl_test_bench_eulerians()
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *Omega, *Chi, *Phi;

	geom = hkl_geometry_factory_new(HKL_GEOMETRY_TYPE_KAPPA6C, 50 * HKL_DEGTORAD);
	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;
	sample = hkl_sample_new("test", HKL_SAMPLE_MONOCRYSTAL);
	engines = hkl_pseudo_axis_engine_list_factory(HKL_GEOMETRY_TYPE_KAPPA6C);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "eulerians");

	Omega = &(((HklParameter *)engine->pseudoAxes[0])->value);
	Chi   = &(((HklParameter *)engine->pseudoAxes[1])->value);
	Phi   = &(((HklParameter *)engine->pseudoAxes[2])->value);

	for(f_idx=0; f_idx<HKL_LIST_LEN(engine->modes); ++f_idx) {
		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		if (f_idx>0)
			engine->mode->parameters[0].value = 1.;

		double omega, chi, phi;
		int res;

		/* studdy this degenerated case */
		*Omega = omega = 0;
		*Chi = chi = 90 * HKL_DEGTORAD;
		*Phi = phi = 0;

		// pseudo -> geometry
		res = hkl_pseudo_axis_engine_setter(engine);
		//hkl_pseudo_axis_engine_fprintf(stdout, engine);

		// geometry -> pseudo
		if (res == HKL_SUCCESS) {
			for(i=0; i<HKL_LIST_LEN(engines->geometries->geometries); ++i) {
				*Omega = *Chi = *Phi = 0;

				hkl_geometry_init_geometry(engine->geometry, engines->geometries->geometries[i]);
				hkl_pseudo_axis_engine_getter(engine);
				//hkl_pseudo_axis_engine_fprintf(stdout, engine);
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_sample_free(sample);
	hkl_detector_free(detector);
	hkl_geometry_free(geom);
}

int main(int argc, char **argv)
{
	size_t i;
	int res = 0;

	hkl_test_bench_k6c();

	return res;
}