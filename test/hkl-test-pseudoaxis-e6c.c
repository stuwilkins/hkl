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
 * Copyright (C) 2003-2010 Synchrotron SOLEIL
 *                         L'Orme des Merisiers Saint-Aubin
 *                         BP 48 91192 GIF-sur-YVETTE CEDEX
 *
 * Authors: Picca Frédéric-Emmanuel <picca@synchrotron-soleil.fr>
 */
#include <hkl.h>

#include "hkl-test.h"

#ifdef HKL_TEST_SUITE_NAME
# undef HKL_TEST_SUITE_NAME
#endif
#define HKL_TEST_SUITE_NAME pseudoaxis_E6C

#define SET_AXES(geometry, mu, omega, chi, phi, gamma, delta) do{	\
		hkl_geometry_set_values_v(geometry, 6,			\
					  mu * HKL_DEGTORAD,		\
					  omega * HKL_DEGTORAD,		\
					  chi * HKL_DEGTORAD,		\
					  phi * HKL_DEGTORAD,		\
					  gamma * HKL_DEGTORAD,		\
					  delta * HKL_DEGTORAD);	\
	} while(0)

#define CHECK_PSEUDOAXES(engine, a, b, c) do{				\
		HklParameter *H = (HklParameter *)(engine->pseudoAxes[0]); \
		HklParameter *K = (HklParameter *)(engine->pseudoAxes[1]); \
		HklParameter *L = (HklParameter *)(engine->pseudoAxes[2]); \
									\
		HKL_ASSERT_DOUBLES_EQUAL(a, H->value, HKL_EPSILON);	\
		HKL_ASSERT_DOUBLES_EQUAL(b, K->value, HKL_EPSILON);	\
		HKL_ASSERT_DOUBLES_EQUAL(c, L->value, HKL_EPSILON);	\
	} while(0)

HKL_TEST_SUITE_FUNC(new)
{
	HklPseudoAxisEngine *engine = hkl_pseudo_axis_engine_e6c_hkl_new();
	hkl_pseudo_axis_engine_free(engine);

	return HKL_TEST_PASS;
}

HKL_TEST_SUITE_FUNC(getter)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN6C);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "hkl");

	/* geometry -> pseudo */
	SET_AXES(geom, 0., 30., 0., 0., 0., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 0., 0., 1.);

	SET_AXES(geom, 0., 30., 0., 90., 0., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 1., 0., 0.);

	SET_AXES(geom, 0., 30, 0., -90., 0., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, -1., 0., 0.);

	SET_AXES(geom, 0., 30., 0., 180., 0., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 0., 0., -1.);

	SET_AXES(geom, 0., 45., 0., 135., 0., 90.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 1., 0., -1.);

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);

	return HKL_TEST_PASS;
}

HKL_TEST_SUITE_FUNC(degenerated)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *H, *K, *L;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN6C);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "hkl");

	H = &(((HklParameter *)engine->pseudoAxes[0])->value);
	K = &(((HklParameter *)engine->pseudoAxes[1])->value);
	L = &(((HklParameter *)engine->pseudoAxes[2])->value);

	for(f_idx=0; f_idx<HKL_LIST_LEN(engine->modes); ++f_idx) {
		double h, k, l;
		int res;

		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		if (HKL_LIST_LEN(engine->mode->parameters))
			engine->mode->parameters[0].value = 0.;

		/* studdy this degenerated case */
		*H = h = 0;
		*K = k = 0;
		*L = l = 1;

		/* pseudo -> geometry */
		res = hkl_pseudo_axis_engine_set(engine, NULL);
		/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */

		/* geometry -> pseudo */
		if (res == HKL_SUCCESS) {
			/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */
			for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i) {
				*H = *K = *L = 0;

				hkl_geometry_init_geometry(geom,
							   engines->geometries->items[i]->geometry);
				hkl_pseudo_axis_engine_get(engine, NULL);

				HKL_ASSERT_DOUBLES_EQUAL(h, *H, HKL_EPSILON);
				HKL_ASSERT_DOUBLES_EQUAL(k, *K, HKL_EPSILON);
				HKL_ASSERT_DOUBLES_EQUAL(l, *L, HKL_EPSILON);
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);

	return HKL_TEST_PASS;
}

HKL_TEST_SUITE_FUNC(q2)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *Q, *Alpha;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN6C);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "q2");

	Q = &(((HklParameter *)engine->pseudoAxes[0])->value);
	Alpha = &(((HklParameter *)engine->pseudoAxes[1])->value);

	/* the init part */
	SET_AXES(geom, 0., 30., 0., 0., 0., 60.);
	hkl_pseudo_axis_engine_initialize(engine, NULL);


	for(f_idx=0; f_idx<HKL_LIST_LEN(engine->modes); ++f_idx){
		double q, alpha;
		int res;

		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		for(q=0.1; q<1.; q += 0.1){
			for(alpha = -M_PI; alpha<M_PI; alpha += M_PI/180.){
				*Q = q;
				*Alpha = alpha;
			
				/* pseudo -> geometry */
				res = hkl_pseudo_axis_engine_set(engine, NULL);
			
				/* geometry -> pseudo */
				if(res == HKL_SUCCESS){
					for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i){
						*Q = 0.;
						*Alpha = 0.;
					
						hkl_geometry_init_geometry(geom,
									   engines->geometries->items[i]->geometry);
						hkl_pseudo_axis_engine_get(engine, NULL);

						HKL_ASSERT_DOUBLES_EQUAL(q, *Q, HKL_EPSILON);
						HKL_ASSERT_DOUBLES_EQUAL(alpha, *Alpha, HKL_EPSILON);
					}
				}
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);

	return HKL_TEST_PASS;
}

HKL_TEST_SUITE_FUNC(petra3)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *hkl;
	HklPseudoAxisEngine *psi;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i;
	double PSI = 90;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN6C);
	geom = hkl_geometry_factory_new(config);
	hkl_source_init(&geom->source, 2.033, 1., 0., 0.);

	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);
	hkl_sample_set_lattice(sample,
			       7.813, 7.813, 7.813,
			       90*HKL_DEGTORAD, 90*HKL_DEGTORAD, 90*HKL_DEGTORAD);
	hkl_matrix_init_from_euler(&sample->U, -112.5 * HKL_DEGTORAD, -87.84 * HKL_DEGTORAD, 157.48 * HKL_DEGTORAD);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	/* set the hkl pseudo axis in psi_constant_vertical */
	hkl = hkl_pseudo_axis_engine_list_get_by_name(engines, "hkl");
	hkl_pseudo_axis_engine_select_mode(hkl, 10);
	hkl_parameter_set_value_unit( (HklParameter *)(hkl->pseudoAxes[0]), 1); /* h */
	hkl_parameter_set_value_unit( (HklParameter *)(hkl->pseudoAxes[1]), 1); /* k */
	hkl_parameter_set_value_unit( (HklParameter *)(hkl->pseudoAxes[2]), 0); /* l */
	hkl_parameter_set_value_unit( &hkl->mode->parameters[0], 0); /* h2 */
	hkl_parameter_set_value_unit( &hkl->mode->parameters[1], 0); /* k2 */
	hkl_parameter_set_value_unit( &hkl->mode->parameters[2], 1); /* l2 */
	hkl_parameter_set_value_unit( &hkl->mode->parameters[3], PSI); /* psi */

	/* set the psi pseudo axis */
	psi = hkl_pseudo_axis_engine_list_get_by_name(engines, "psi");
	hkl_pseudo_axis_engine_select_mode(psi, 10);
	hkl_parameter_set_value_unit( &psi->mode->parameters[0], 0); /* h2 */
	hkl_parameter_set_value_unit( &psi->mode->parameters[1], 0); /* k2 */
	hkl_parameter_set_value_unit( &psi->mode->parameters[2], 1); /* l2 */

	/* Compute the hkl [1, 1, 0] in psi_constant_vertical mode with */
	/* h2,k2,l2= [0, 0,1] and psi = 90 */
	if( HKL_SUCCESS == hkl_pseudo_axis_engine_set(hkl, NULL)){
		for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i) {
			hkl_geometry_init_geometry(geom,
						   engines->geometries->items[i]->geometry);
			hkl_pseudo_axis_engine_initialize(psi, NULL);
			hkl_pseudo_axis_engine_list_get(engines);
			HKL_ASSERT_DOUBLES_EQUAL(PSI * HKL_DEGTORAD, ((HklParameter *)(psi->pseudoAxes[0]))->value, HKL_EPSILON);
			/* hkl_pseudo_axis_engine_list_fprintf(stdout, engines); */
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);

	return HKL_TEST_PASS;
}

HKL_TEST_SUITE_BEGIN

HKL_TEST( new );
HKL_TEST( getter );
HKL_TEST( degenerated );
HKL_TEST( q2 );
HKL_TEST( petra3 );

HKL_TEST_SUITE_END