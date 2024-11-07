/*
 *	Copyright (c) 2024, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "common.h"


#define kDefaultInputDistributionConstantAnnualInterestRateMin	(0.5)
#define kDefaultInputDistributionConstantAnnualInterestRateMax	(1.0)
#define kDefaultInputDistributionConstantAnnualContributionMin	(5000.0)
#define kDefaultInputDistributionConstantAnnualContributionMax	(10000.0)
#define kDefaultInputDistributionConstantTaxRateInterestMin	(20.0)
#define kDefaultInputDistributionConstantTaxRateInterestMax	(40.0)
#define kDefaultInputDistributionConstantWithdrawalRateMin	(20.0)
#define kDefaultInputDistributionConstantWithdrawalRateMax	(40.0)

typedef enum
{
	kDemoFinanceIraDefaultNumberOfYearsToRetirement = 20,
} DemoFinanceIraDefault;

typedef enum
{
	kInputDistributionIndexTotalAnnualContributionToAccount	= 0,
	kInputDistributionIndexCompoundedAnnualInterestRate	= 1,
	kInputDistributionIndexWithdrawalRate			= 2,
	kInputDistributionIndexAssumedTaxRateOnInterest		= 3,
	kInputDistributionIndexMax				= 4
} InputDistributionIndex;

typedef enum
{
	kOutputDistributionIndexFutureValueTaxed		= 0,
	kOutputDistributionIndexFutureValueTaxedWithdrawal	= 1,
	kOutputDistributionIndexMax				= 2
} OutputDistributionIndex;

typedef struct
{
	CommonCommandLineArguments	common;

	int				numberOfYearsToRetirement;
	char				inputVariablesUxStrings[kInputDistributionIndexMax][kCommonConstantMaxCharsPerLine];
	bool				isInputVariableSet[kInputDistributionIndexMax];

} CommandLineArguments;

/**
 *	@brief	Print out command-line usage.
 */
void	printUsage(void);

/**
 *	@brief	Get command-line arguments.
 *
 *	@param	argc		: Argument count from `main()`.
 *	@param	argv		: Argument vector from `main()`.
 *	@param	arguments	: Pointer to struct to store arguments.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType	getCommandLineArguments(
					int			argc,
					char *			argv[],
					CommandLineArguments *	arguments);

/**
 *	@brief	Set distributions for input variables via UxHw calls.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	inputVariables			: The input variables to be set.
 */
void	setInputVariables(
		CommandLineArguments *	arguments,
		double *		inputVariables[kInputDistributionIndexMax]);


/**
 *	@brief	Determine the index range of selected outputs.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	pointerToOutputSelectLowerBound	: Pointer to lower bound index.
 *	@param	pointerToOutputSelectUpperBound	: Pointer to upper bound index.
 */
void	determineIndexRangeOfSelectedOutputs(
		CommandLineArguments *		arguments,
		OutputDistributionIndex *	pointerToOutputSelectLowerBound,
		OutputDistributionIndex *	pointerToOutputSelectUpperBound);

/**
 *	@brief	Print human-consumable output.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	outputDistributions		: The output variables.
 *	@param	outputNames			: Names of the output variables to print.
 *	@param	outputVariableDescriptions	: Descriptions of output variables to print.
 *	@param	monteCarloOutputSamples		: Monte Carlo output samples that will populate JSON struct values if in Monte Carlo mode.
 */
void	printHumanConsumableOutput(
		CommandLineArguments *	arguments,
		double *		outputDistributions,
		const char *		outputVariableNames[kOutputDistributionIndexMax],
		const char *		outputVariableDescriptions[kOutputDistributionIndexMax],
		double *		monteCarloOutputSamples);

/**
 *	@brief	Populate and print JSON variables.
 *
 *	@param	jsonVariables			: Array of `JSONVariable` structs to populate and print.
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	outputDistributions		: The output variables.
 *	@param	outputVariableDescriptions	: Descriptions of output variables from which the array of `JSONVariable` structs will take their descriptions.
 *	@param	monteCarloOutputSamples		: Monte Carlo output samples that will populate `JSONVariable` struct values if in Monte Carlo mode.
 */
void	populateAndPrintJSONVariables(
		CommandLineArguments *	arguments,
		double *		inputVariables[kInputDistributionIndexMax],
		const char *		inputVariableDescriptions[kOutputDistributionIndexMax],
		double *		outputDistributions,
		const char *		outputVariableDescriptions[kOutputDistributionIndexMax],
		double *		monteCarloOutputSamples);

/**
 *	@brief	Read the input variables from a CSV file.
 *
 *	@param	arguments		: Pointer to command-line arguments struct.
 *	@param	CSVInputVariables	: The input variables to be set.
 *	@return				: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType	prepareCSVInputVariables(
					CommandLineArguments *	arguments,
					double *		CSVInputVariables[kInputDistributionIndexMax]);
