/////////////////////////////////////////////////
// ConversionFactors
//
// Methods for converting to and from metric and
// imperial measurement units
//
// M.Parks							15-09-2000
// Based loosely on the Sigmat 9000NT implementation.
////////////////////////////////////////////////

#include "ConversionFactors.h"

// Global variables
extern	structMeasurementUnits	g_MeasurementUnits;


double MetricToImperial( double fData, int nUnits )
{
	double	fMultiplier = 1.0f;

	// Load the multiplier
	switch( nUnits )
	{
	case WIDTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWidthUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = WIDTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case GAUGE_DIMENSIONS:
		switch( g_MeasurementUnits.m_nGaugeUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = GAUGE_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case LENGTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nLengthUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = LENGTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case WEIGHT_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = WEIGHT_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case THROUGHPUT_DIMENSIONS:
		switch( g_MeasurementUnits.m_nThroughputUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = THROUGHPUT_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case WEIGHT_PER_LENGTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightLengthUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = WEIGHT_PER_LENGTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case WEIGHT_PER_AREA_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightAreaUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = WEIGHT_PER_AREA_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case LINE_SPEED_DIMENSIONS:	
		switch( g_MeasurementUnits.m_nLineSpeedUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = LINE_SPEED_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case DENSITY_DIMENSIONS:	
		switch( g_MeasurementUnits.m_nDensityUnits )
		{
		case UNIT_IMPERIAL:
			fMultiplier = DENSITY_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fMultiplier = 1.0f;
			break;
		}
		break;
	case NO_DIMENSIONS:
	default:
		fMultiplier = 1.0f;
		break;
	}

	// If the multiplier is not 1 then apply it to the
	// variable.
	if( fMultiplier != 1.0f )
		fData *= fMultiplier;

	return fData;
}


double ImperialToMetric( double fData, int nUnits )
{
	double	fDivisor = 1.0f;

	// Load the multiplier
	switch( nUnits )
	{
	case WIDTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWidthUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = WIDTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case GAUGE_DIMENSIONS:
		switch( g_MeasurementUnits.m_nGaugeUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = GAUGE_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case LENGTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nLengthUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = LENGTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case WEIGHT_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = WEIGHT_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case THROUGHPUT_DIMENSIONS:
		switch( g_MeasurementUnits.m_nThroughputUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = THROUGHPUT_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case WEIGHT_PER_LENGTH_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightLengthUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = WEIGHT_PER_LENGTH_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case WEIGHT_PER_AREA_DIMENSIONS:
		switch( g_MeasurementUnits.m_nWeightAreaUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = WEIGHT_PER_AREA_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case LINE_SPEED_DIMENSIONS:	
		switch( g_MeasurementUnits.m_nLineSpeedUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = LINE_SPEED_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case DENSITY_DIMENSIONS:	
		switch( g_MeasurementUnits.m_nDensityUnits )
		{
		case UNIT_IMPERIAL:
			fDivisor = DENSITY_CONVERSION_FACTOR;
			break;
		case UNIT_METRIC:
		default:
			fDivisor = 1.0f;
			break;
		}
		break;
	case NO_DIMENSIONS:
	default:
		fDivisor = 1.0f;
		break;
	}

	// If the multiplier is not 1 then apply it to the
	// variable.
	if( fDivisor != 1.0f )
		fData /= fDivisor;

	return fData;
}

