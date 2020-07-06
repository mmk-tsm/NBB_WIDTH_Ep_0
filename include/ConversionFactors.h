/////////////////////////////////////////////////
// ConversionFactors.h
//
// Constants used for converting between metric
// and imperial measurement units.
//
// M.Parks							15-09-2000
// Based loosely on the Sigmat 9000NT implementation.
//
// P.Smith							14/11/06
// added UNIT_IMPERIAL_MODBUS
//
// P.Smith							6/11/07
// defined LAST_UNITS_MODE
////////////////////////////////////////////////

#ifndef __CONVERSIONFACTORS_H__
#define __CONVERSIONFACTORS_H__

// Function declarations
double	MetricToImperial( double fData, int nUnits );
double	ImperialToMetric( double fData, int nUnits );


// Different dimension types supported
#define	WIDTH_DIMENSIONS				0
#define	GAUGE_DIMENSIONS				1
#define	LENGTH_DIMENSIONS				2
#define	WEIGHT_DIMENSIONS				3
#define	THROUGHPUT_DIMENSIONS			4
#define	WEIGHT_PER_LENGTH_DIMENSIONS	5
#define	WEIGHT_PER_AREA_DIMENSIONS		6
#define	LINE_SPEED_DIMENSIONS			7
#define	DENSITY_DIMENSIONS				8
#define	NO_DIMENSIONS					9


// Different measurement bases supported
#define	UNIT_METRIC				0
#define	UNIT_IMPERIAL			1
#define	UNIT_IMPERIAL_MODBUS    2

#define LAST_UNITS_MODE      UNIT_IMPERIAL_MODBUS


// Conversion factors for each of the dimensions.
// These are applied to the metric value as a multiplier.
#define	WIDTH_CONVERSION_FACTOR				0.03937f
#define	GAUGE_CONVERSION_FACTOR				0.03937f
#define	LENGTH_CONVERSION_FACTOR			3.2808f
#define	WEIGHT_CONVERSION_FACTOR			2.2046f
#define	THROUGHPUT_CONVERSION_FACTOR		2.2046f
#define	WEIGHT_PER_LENGTH_CONVERSION_FACTOR	0.6719f
#define	WEIGHT_PER_AREA_CONVERSION_FACTOR	1.0f		// No idea what this should be
#define	LINE_SPEED_CONVERSION_FACTOR		3.2808f
#define	DENSITY_CONVERSION_FACTOR			62.43f



// Data structure used to hold the measurement base settings
// for each of the supported dimension types
typedef struct {
	int		m_nWidthUnits;
	int		m_nGaugeUnits;
	int		m_nLengthUnits;
	int		m_nWeightUnits;
	int		m_nThroughputUnits;
	int		m_nWeightLengthUnits;
	int		m_nWeightAreaUnits;
	int		m_nLineSpeedUnits;
	int		m_nDensityUnits;
} structMeasurementUnits;

#endif	//CONVERSIONFACTORS_H
