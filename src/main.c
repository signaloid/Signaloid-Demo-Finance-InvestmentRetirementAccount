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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "uxhw.h"
#include "common.h"
#include "utilities.h"
#include "kernel.h"


int
main(int argc, char *  argv[])
{
	CommandLineArguments	arguments = {0};
	clock_t			start = 0;
	clock_t			end = 0;
	double			cpuTimeUsedInSeconds;
	double			benchmarkOutput;
	double *		inputVariables[kInputDistributionIndexMax];
	double			outputDistributions[kOutputDistributionIndexMax];
	const char *		inputVariableDescriptions[kInputDistributionIndexMax] =
				{
					"Total annual contribution",
					"Compounded annual interest percentage",
					"Withdrawal rate percentage",
					"Assumed tax rate on interest percentage"
				};
	const char *		outputVariableNames[kOutputDistributionIndexMax] =
				{
					"futureValueTaxed",
					"futureValueTaxFreeWithWithdrawalTax"
				};

	const char *		outputVariableDescriptions[kOutputDistributionIndexMax] =
				{
					"Future value, for yearly taxable payments (compounded annually)",
					"Future value, for yearly tax-free payments and taxed withdrawal-events (compounded annually)"
				};

	double *		monteCarloOutputSamples = NULL;
	int			numberOfYearsToRetirement;
	MeanAndVariance		monteCarloOutputMeanAndVariance = {0};

	if (getCommandLineArguments(argc, argv, &arguments) != kCommonConstantReturnTypeSuccess)
	{
		return EXIT_FAILURE;
	}

	/*
	 *	Read input distributions from CSV if input from file is enabled.
	 */
	if (arguments.common.isInputFromFileEnabled)
	{
		if (prepareCSVInputVariables(&arguments, inputVariables) != kCommonConstantReturnTypeSuccess)
		{
			return EXIT_FAILURE;
		}
	}

	/*
	 *	Number of years to retirement is always from arguments.
	 */
	numberOfYearsToRetirement = arguments.numberOfYearsToRetirement;

	for (size_t i = 0; i < kInputDistributionIndexMax; i++)
	{
		inputVariables[i] = (double *) checkedMalloc(numberOfYearsToRetirement * sizeof(double), __FILE__, __LINE__);
	}

	/*
	 *	Allocate for `monteCarloOutputSamples` if in Monte Carlo mode.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		monteCarloOutputSamples = (double *) checkedMalloc(
			arguments.common.numberOfMonteCarloIterations * sizeof(double),
			__FILE__,
			__LINE__);
	}

	/*
	 *	Start timing if timing is enabled or in benchmarking mode.
	 */
	if ((arguments.common.isTimingEnabled) || (arguments.common.isBenchmarkingMode))
	{
		start = clock();
	}

	/*
	 *	Execute process kernel in a loop. The size of loop is 1 unless in Monte Carlo mode.
	 */
	for (size_t i = 0; i < arguments.common.numberOfMonteCarloIterations; ++i)
	{
		/*
		 *	Set inputs via UxHw calls if input from file is not enabled.
		 */
		if (!arguments.common.isInputFromFileEnabled)
		{
			setInputVariables(&arguments, inputVariables);
		}

		/*
		 *	Execute process kernel.
		 */
		calculateOutput(&arguments, numberOfYearsToRetirement, inputVariables, outputDistributions);

		/*
		 *	If in Monte Carlo mode, populate `monteCarloOutputSamples`.
		 */
		if (arguments.common.isMonteCarloMode)
		{
			monteCarloOutputSamples[i] = outputDistributions[arguments.common.outputSelect];
		}
		/*
		 *	Else, if in benchmarking mode, populate `benchmarkOutput`.
		 */
		else if (arguments.common.isBenchmarkingMode)
		{
			benchmarkOutput = outputDistributions[arguments.common.outputSelect];
		}
	}

	/*
	 *	If not doing Laplace version, then approximate the cost of the third phase of
	 *	Monte Carlo (post-processing), by calculating the mean and variance.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		monteCarloOutputMeanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
								monteCarloOutputSamples,
								arguments.common.numberOfMonteCarloIterations);
		benchmarkOutput = monteCarloOutputMeanAndVariance.mean;
	}

	/*
	 *	Stop timing if timing is enabled or in benchmarking mode.
	 */
	if ((arguments.common.isTimingEnabled) || (arguments.common.isBenchmarkingMode))
	{
		end = clock();
		cpuTimeUsedInSeconds = ((double)(end - start)) / CLOCKS_PER_SEC;
	}

	/*
	 *	If in benchmarking mode, print timing result in a special format:
	 *		(1) Benchmark output (for calculating Wasserstein distance to reference)
	 *		(2) Time in microseconds
	 */
	if (arguments.common.isBenchmarkingMode)
	{
		printf("%lf %" PRIu64 "\n", benchmarkOutput, (uint64_t)(cpuTimeUsedInSeconds * 1000000));
	}
	/*
	 *	If not in benchmarking mode...
	 */
	else
	{
		/*
		 *	Print json outputs if in JSON output mode.
		 */
		if (arguments.common.isOutputJSONMode)
		{
			populateAndPrintJSONVariables(
				&arguments,
				inputVariables,
				inputVariableDescriptions,
				outputDistributions,
				outputVariableDescriptions,
				monteCarloOutputSamples);
		}
		/*
		 *	Print human-consumable output if not in JSON output mode.
		 */
		else
		{
			printHumanConsumableOutput(
				&arguments,
				outputDistributions,
				outputVariableNames,
				outputVariableDescriptions,
				monteCarloOutputSamples);
		}

		/*
		 *	Print timing if timing is enabled.
		 */
		if (arguments.common.isTimingEnabled)
		{
			printf("\nCPU time used: %lf seconds\n", cpuTimeUsedInSeconds);
		}
	}

	/*
	 *	Save Monte Carlo data to "data.out" if in Monte Carlo mode.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples,
			(uint64_t)(cpuTimeUsedInSeconds * 1000000),
			arguments.common.numberOfMonteCarloIterations);
	}
	/*
	 *	Save outputs to file if not in Monte Carlo mode and write to file is enabled.
	 */
	else
	{
		if (arguments.common.isWriteToFileEnabled)
		{
			if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputDistributions,
				outputVariableNames,
				kOutputDistributionIndexMax))
			{
				fprintf(stderr, "Error: Could not write to output CSV file \"%s\".\n", arguments.common.outputFilePath);
				return EXIT_FAILURE;
			}
		}
	}

	/*
	 *	Free allocations.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		free(monteCarloOutputSamples);
	}

	for (size_t i = 0; i < kInputDistributionIndexMax; i++)
	{
		free(inputVariables[i]);
	}

	return EXIT_SUCCESS;
}
