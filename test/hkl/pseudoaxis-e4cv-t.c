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
#include <tap/basic.h>

#define SET_AXES(geom, omega, chi, phi, tth) do{		\
		hkl_geometry_set_values_v(geom, 4,		\
					  omega * HKL_DEGTORAD,	\
					  chi * HKL_DEGTORAD,	\
					  phi * HKL_DEGTORAD,	\
					  tth * HKL_DEGTORAD);	\
	} while(0)

#define CHECK_PSEUDOAXES(engine, a, b, c) do{				\
		HklParameter *H = (HklParameter *)(engine->pseudoAxes[0]); \
		HklParameter *K = (HklParameter *)(engine->pseudoAxes[1]); \
		HklParameter *L = (HklParameter *)(engine->pseudoAxes[2]); \
									\
		is_double(a, H->value, HKL_EPSILON, __func__);	\
		is_double(b, K->value, HKL_EPSILON, __func__);	\
		is_double(c, L->value, HKL_EPSILON, __func__);	\
	} while(0)

static void new(void)
{
	HklPseudoAxisEngine *engine = hkl_pseudo_axis_engine_e4c_hkl_new();
	hkl_pseudo_axis_engine_free(engine);
}

static void getter(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "hkl");

	/* geometry -> pseudo */
	SET_AXES(geom, 30., 0., 0., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 0., 0., 1.);

	SET_AXES(geom, 30., 0., 90., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 1., 0., 0.);

	SET_AXES(geom, 30, 0., -90., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, -1., 0., 0.);

	SET_AXES(geom, 30., 0., 180., 60.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 0., 0., -1.);

	SET_AXES(geom, 45., 0., 135., 90.);
	hkl_pseudo_axis_engine_get(engine, NULL);
	CHECK_PSEUDOAXES(engine, 1., 0., -1.);

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

static void degenerated(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *H, *K, *L;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
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

	for(f_idx=0; f_idx<engine->modes_len; ++f_idx){
		double h, k, l;
		int res;

		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		if (engine->mode->parameters_len)
			engine->mode->parameters[0].value = 0.;

		/* studdy this degenerated case */
		*H = h = 0;
		*K = k = 0;
		*L = l = 1;

		/* pseudo -> geometry */
		res = hkl_pseudo_axis_engine_set(engine, NULL);
		/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */

		/* geometry -> pseudo */
		if(res == HKL_SUCCESS){
			/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */
			for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i){
				*H = *K = *L = 0;

				hkl_geometry_init_geometry(engines->geometry,
							   engines->geometries->items[i].geometry);
				hkl_pseudo_axis_engine_get(engine, NULL);

				is_double(h, *H, HKL_EPSILON, __func__);
				is_double(k, *K, HKL_EPSILON, __func__);
				is_double(l, *L, HKL_EPSILON, __func__);
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

static void psi_getter(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *psi;
	double *h_ref;
	double *k_ref;
	double *l_ref;
	int status;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "psi");

	psi = &(((HklParameter *)engine->pseudoAxes[0])->value);
	h_ref = &engine->mode->parameters[0].value;
	k_ref = &engine->mode->parameters[1].value;
	l_ref = &engine->mode->parameters[2].value;

	/* the getter part */
	SET_AXES(geom, 30., 0., 0., 60.);
	hkl_pseudo_axis_engine_initialize(engine, NULL);

	*h_ref = 1;
	*k_ref = 0;
	*l_ref = 0;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_SUCCESS == status, __func__);
	is_double(0 * HKL_DEGTORAD, *psi, HKL_EPSILON, __func__);

	*h_ref = 0;
	*k_ref = 1;
	*l_ref = 0;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_SUCCESS == status, __func__);
	is_double(90 * HKL_DEGTORAD, *psi, HKL_EPSILON, __func__);

	/* here Q and <h, k, l>_ref are colinear */
	*h_ref = 0;
	*k_ref = 0;
	*l_ref = 1;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_FAIL == status, __func__);

	*h_ref = -1;
	*k_ref = 0;
	*l_ref = 0;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_SUCCESS == status, __func__);
	is_double(180 * HKL_DEGTORAD, *psi, HKL_EPSILON, __func__);

	*h_ref = 0;
	*k_ref = -1;
	*l_ref = 0;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_SUCCESS == status, __func__);
	is_double(-90 * HKL_DEGTORAD, *psi, HKL_EPSILON, __func__);
	
	*h_ref = 0;
	*k_ref = 0;
	*l_ref = -1;
	status = hkl_pseudo_axis_engine_get(engine, NULL);
	ok(HKL_FAIL == status, __func__);

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

static void psi_setter(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *Psi;
	double *h_ref, *k_ref, *l_ref;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "psi");

	Psi = &(((HklParameter *)engine->pseudoAxes[0])->value);
	h_ref = &engine->mode->parameters[0].value;
	k_ref = &engine->mode->parameters[1].value;
	l_ref = &engine->mode->parameters[2].value;

	/* the init part */
	SET_AXES(geom, 30., 0., 0., 60.);
	*h_ref = 1;
	*k_ref = 0;
	*l_ref = 0;
	hkl_pseudo_axis_engine_initialize(engine, NULL);


	for(f_idx=0; f_idx<engine->modes_len; ++f_idx){
		double psi;
		int res;

		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		for(psi=-180;psi<180;psi++){
			*Psi = psi * HKL_DEGTORAD;
			
			/* pseudo -> geometry */
			res = hkl_pseudo_axis_engine_set(engine, NULL);
			/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */
			
			/* geometry -> pseudo */
			if(res == HKL_SUCCESS){
				/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */
				for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i){
					*Psi = 0;
					
					hkl_geometry_init_geometry(geom,
								   engines->geometries->items[i].geometry);
					hkl_pseudo_axis_engine_get(engine, NULL);
					is_double(psi * HKL_DEGTORAD, *Psi, HKL_EPSILON, __func__);
				}
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

static void q(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *Q;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
	geom = hkl_geometry_factory_new(config);
	sample = hkl_sample_new("test", HKL_SAMPLE_TYPE_MONOCRYSTAL);

	detector = hkl_detector_factory_new(HKL_DETECTOR_TYPE_0D);
	detector->idx = 1;

	engines = hkl_pseudo_axis_engine_list_factory(config);
	hkl_pseudo_axis_engine_list_init(engines, geom, detector, sample);

	engine = hkl_pseudo_axis_engine_list_get_by_name(engines, "q");

	Q = &(((HklParameter *)engine->pseudoAxes[0])->value);

	/* the init part */
	SET_AXES(geom, 30., 0., 0., 60.);
	hkl_pseudo_axis_engine_initialize(engine, NULL);


	for(f_idx=0; f_idx<engine->modes_len; ++f_idx){
		double q;
		int res;

		hkl_pseudo_axis_engine_select_mode(engine, f_idx);
		for(q=-1.; q<1.; q += 0.1){
			*Q = q;
			
			/* pseudo -> geometry */
			res = hkl_pseudo_axis_engine_set(engine, NULL);
			
			/* geometry -> pseudo */
			if(res == HKL_SUCCESS){
				for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i){
					*Q = 0;
					
					hkl_geometry_init_geometry(geom,
								   engines->geometries->items[i].geometry);
					hkl_pseudo_axis_engine_get(engine, NULL);
					
					is_double(q, *Q, HKL_EPSILON, __func__);
				}
			}
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

static void hkl_psi_constant_vertical(void)
{
	HklPseudoAxisEngineList *engines;
	HklPseudoAxisEngine *engine;
	const HklGeometryConfig *config;
	HklGeometry *geom;
	HklDetector *detector;
	HklSample *sample;
	size_t i, f_idx;
	double *H, *K, *L;
	double h, k, l;
	double *h_ref, *k_ref, *l_ref, *psi_ref;
	int res;

	config = hkl_geometry_factory_get_config_from_type(HKL_GEOMETRY_TYPE_EULERIAN4C_VERTICAL);
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


	hkl_pseudo_axis_engine_select_mode(engine, 5);
	h_ref = &engine->mode->parameters[0].value;
	k_ref = &engine->mode->parameters[1].value;
	l_ref = &engine->mode->parameters[2].value;
	psi_ref = &engine->mode->parameters[3].value;

	/* the init part */
	SET_AXES(geom, 30., 0., 0., 60.);
	*h_ref = 1;
	*k_ref = 1;
	*l_ref = 0;
	hkl_pseudo_axis_engine_initialize(engine, NULL);

	*H = h = 1;
	*K = k = 0;
	*L = l = 1;
	res = hkl_pseudo_axis_engine_set(engine, NULL);
			
	/* geometry -> pseudo */
	if(res == HKL_SUCCESS){
		/* hkl_pseudo_axis_engine_fprintf(stdout, engine); */
		for(i=0; i<hkl_geometry_list_len(engines->geometries); ++i){
			*H = *K = *L = 0;
					
			hkl_geometry_init_geometry(geom,
						   engines->geometries->items[i].geometry);
			hkl_pseudo_axis_engine_get(engine, NULL);
			is_double(h, *H, HKL_EPSILON, __func__);
			is_double(k, *K, HKL_EPSILON, __func__);
			is_double(l, *L, HKL_EPSILON, __func__);
		}
	}

	hkl_pseudo_axis_engine_list_free(engines);
	hkl_detector_free(detector);
	hkl_sample_free(sample);
	hkl_geometry_free(geom);
}

int main(int argc, char** argv)
{
	plan(1363);

	new();
	getter();
	degenerated();
	psi_getter();
	psi_setter();
	q();
	hkl_psi_constant_vertical();

	return 0;
}