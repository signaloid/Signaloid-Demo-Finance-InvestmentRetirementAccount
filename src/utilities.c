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

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <uxhw.h>
#include "utilities.h"


/**
 *	@brief	Return the default distributional value for the compounded annual interest rate.
 *
 *	@return	: The distributional value for the compounded annual interest rate.
 */
static double
getDefaultCompoundedAnnualInterestRate()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantAnnualInterestRateMin,
		kDefaultInputDistributionConstantAnnualInterestRateMax);
}

/**
 *	@brief	Return the default distributional value for the total annual contribution to account.
 *
 *	@return	: The distributional value for the total annual contribution to account.
 */
static double
getDefaultTotalAnnualContributionToAccount()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantAnnualContributionMin,
		kDefaultInputDistributionConstantAnnualContributionMax);
}

/**
 *	@brief	Return the default distributional value for the assumed tax rate on interest.
 *
 *	@return	: The distributional value for the assumed tax rate on interest.
 */
static double
getDefaultAssumedTaxRateOnInterest()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantTaxRateInterestMin,
		kDefaultInputDistributionConstantTaxRateInterestMax);
}

/**
 *	@brief	Return the default distributional value for withdrawal rate.
 *
 *	@return	: The distributional value for the withdrawal rate.
 */
static double
getDefaultWithdrawalRate()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantWithdrawalRateMin,
		kDefaultInputDistributionConstantWithdrawalRateMax);
}

void
setInputVariables(
	CommandLineArguments *	arguments,
	double *		inputVariables[kInputDistributionIndexMax])
{
	for (int i = 0; i < arguments->numberOfYearsToRetirement; i++)
	{
		/*
		 *	Check if the input variables are set using command-line arguments.
		 *	If no, set the default values using UxHw calls.
		 *	If yes, set the values by scanning the input strings obtained from the command-line.
		 *
		 *	Note: To get independent copies for a distribution specified from the command-line as
		 *	a Ux string, we used to use `UxHwDoubleCopyDistShape()`. This method worked as intended
		 *	for non-reference Laplace cores, however it did not work correctly for C0-Reference
		 *	(or for the native Monte Carlo (via `compat` submodule). So, instead, we now propagate
		 *	the input strings from the command-line (instead of the corresponding distribution) and
		 *	use `scanf()` at each iteration of this loop. This way C0-Reference also works correctly.
		 */
		if(arguments->isInputVariableSet[kInputDistributionIndexTotalAnnualContributionToAccount])
		{
			sscanf(arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount], "%lf", &inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount][i]);
		}
		else
		{
			inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount][i] = getDefaultTotalAnnualContributionToAccount();
		}

		if(arguments->isInputVariableSet[kInputDistributionIndexCompoundedAnnualInterestRate])
		{
			sscanf(arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate], "%lf", &inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate][i]);
		}
		else
		{
			inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate][i] = getDefaultCompoundedAnnualInterestRate();
		}

		if(arguments->isInputVariableSet[kInputDistributionIndexAssumedTaxRateOnInterest])
		{
			sscanf(arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest], "%lf", &inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest][i]);
		}
		else
		{
			inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest][i] = getDefaultAssumedTaxRateOnInterest();
		}

		if(arguments->isInputVariableSet[kInputDistributionIndexWithdrawalRate])
		{
			sscanf(arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate], "%lf", &inputVariables[kInputDistributionIndexWithdrawalRate][i]);
		}
		else
		{
			inputVariables[kInputDistributionIndexWithdrawalRate][i] = getDefaultWithdrawalRate();
		}
	}

	return;
}

/**
 *	@brief	Determine the index range of selected outputs.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	pointerToOutputSelectLowerBound	: Pointer to lower bound index.
 *	@param	pointerToOutputSelectUpperBound	: Pointer to upper bound index.
 */
void
determineIndexRangeOfSelectedOutputs(
	CommandLineArguments *		arguments,
	OutputDistributionIndex *	pointerToOutputSelectLowerBound,
	OutputDistributionIndex *	pointerToOutputSelectUpperBound)
{
	/*
	 *	If outputSelect is equal to kOutputDistributionIndexMax, index range is the full range.
	 */
	if (arguments->common.outputSelect == kOutputDistributionIndexMax)
	{
		*pointerToOutputSelectLowerBound = (OutputDistributionIndex)0;
		*pointerToOutputSelectUpperBound = kOutputDistributionIndexMax;
	}
	/*
	 *	Else, index range lower bound is the index of the selected output and the length of range is 1 (single output selected).
	 */
	else
	{
		*pointerToOutputSelectLowerBound = arguments->common.outputSelect;
		*pointerToOutputSelectUpperBound = *pointerToOutputSelectLowerBound + 1;
	}

	return;
}

/**
 *	@brief	Set the default values for the command-line arguments.
 *
 *	@param	arguments	: command-line arguments pointer.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
static CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments *  arguments)
{
	if (arguments == NULL)
	{
		fprintf(stderr, "Error: Arguments pointer is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

/*
 *	Older GCC versions have a bug which gives a spurious warning for the C universal zero
 *	initializer `{0}`. Any workaround makes the code less portable or prevents the common code
 *	from adding new fields to the `CommonCommandLineArguments` struct. Therefore, we surpress
 *	this warning.
 *
 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

	memset(&arguments->common, 0, sizeof(CommonCommandLineArguments));

	arguments->numberOfYearsToRetirement = kDemoFinanceIraDefaultNumberOfYearsToRetirement;

	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate],
		kCommonConstantMaxCharsPerFilepath,
		"%s",
		"");
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount],
		kCommonConstantMaxCharsPerFilepath,
		"%s",
		"");
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest],
		kCommonConstantMaxCharsPerFilepath,
		"%s",
		"");
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate],
		kCommonConstantMaxCharsPerFilepath,
		"%s",
		"");

#pragma GCC diagnostic pop

	return kCommonConstantReturnTypeSuccess;
}

void
printUsage(void)
{
	fprintf(stderr,
		"Example: Personal Finance: Tax-Free Retirement Account (IRA) or Keogh Plan - "
		"Signaloid version\n");
	fprintf(stderr, "\n");

	fprintf(stderr, "Usage: Valid command-line arguments are:\n");
	fprintf(
		stderr,
		"\t[-o, --output <Path to output CSV file : str>] (Specify the output file.)\n"
		"\t[-S, --select-output <output : int> (Default: 0)] (Compute 0-indexed output.)\n"
		"\t[-M, --multiple-executions <Number of executions : int> (Default: 1)] (Repeated execute kernel for benchmarking.)\n"
		"\t[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)\n"
		"\t[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n"
		"\t[-n, --number-of-years <The number of years to retirement : int in [0, inf)> (Default: %d)]\n"
		"\t[-c, --compounded-annual-interest-rate <The compounded annual interest rate expressed as a percentage: double> (Default: Uniform(%"SignaloidParticleModifier".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-t, --total-annual-contribution-to-account <The total annual contribution to the account : double> (Default: Uniform(%"SignaloidParticleModifier".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-r, --assumed-tax-rate-on-interest <The assumed tax rate on interest expressed as a percentage : double> (Default: Uniform(%"SignaloidParticleModifier".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-w, --withdrawal-rate <The withdrawal rate expressed as a percentage : double> (Default: Uniform(%"SignaloidParticleModifier".1f, %"SignaloidParticleModifier".1f))]\n",
		kDemoFinanceIraDefaultNumberOfYearsToRetirement,
		kDefaultInputDistributionConstantAnnualInterestRateMin,
		kDefaultInputDistributionConstantAnnualInterestRateMax,
		kDefaultInputDistributionConstantAnnualContributionMin,
		kDefaultInputDistributionConstantAnnualContributionMax,
		kDefaultInputDistributionConstantTaxRateInterestMin,
		kDefaultInputDistributionConstantTaxRateInterestMax,
		kDefaultInputDistributionConstantWithdrawalRateMin,
		kDefaultInputDistributionConstantWithdrawalRateMax);

	fprintf(stderr, "\n");

	return;
}

CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments)
{
	const char *	numberOfYearsToRetirementArg = NULL;
	const char *	compoundedAnnualInterestRateArg = NULL;
	const char *	totalAnnualContributionToAccountArg = NULL;
	const char *	assumedTaxRateOnInterestArg = NULL;
	const char *	withdrawalRateArg = NULL;
	bool 		distributionalArgumentGiven = false;
	const char	kConstantStringUx[] = "Ux";

	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	DemoOption	options[] =
	{
		{ .opt = "n", .optAlternative = "number-of-years",			.hasArg = true, .foundArg = &numberOfYearsToRetirementArg,		.foundOpt = NULL },
		{ .opt = "c", .optAlternative = "compounded-annual-interest-rate",	.hasArg = true, .foundArg = &compoundedAnnualInterestRateArg,		.foundOpt = NULL },
		{ .opt = "t", .optAlternative = "total-annual-contribution-to-account",	.hasArg = true, .foundArg = &totalAnnualContributionToAccountArg,	.foundOpt = NULL },
		{ .opt = "r", .optAlternative = "assumed-tax-rate-on-interest",		.hasArg = true, .foundArg = &assumedTaxRateOnInterestArg,		.foundOpt = NULL },
		{ .opt = "w", .optAlternative = "withdrawal-rate",			.hasArg = true, .foundArg = &withdrawalRateArg,				.foundOpt = NULL },
		{0},
	};

	if (parseArgs(argc, argv, &arguments->common, options) != kCommonConstantReturnTypeSuccess)
	{
		fprintf(stderr, "Parsing command-line arguments failed\n");
		printUsage();

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	if (((strcmp(arguments->common.outputFilePath, "") != 0) || arguments->common.isWriteToFileEnabled) && (!arguments->common.isOutputJSONMode))
	{
		printf("The output will be stored in the file: %s\n", arguments->common.outputFilePath);
	}

	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputDistributionIndexMax;
	}
	else if (arguments->common.outputSelect >= kOutputDistributionIndexMax)
	{
		fprintf(stderr, "Error: Output select option must be in the range [0, %d].\n", kOutputDistributionIndexMax-1);

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Warning: Verbose mode not supported. Continuing in non-verbose mode.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	When all outputs are selected, we cannot be in benchmarking mode or Monte Carlo mode.
	 */
	if (arguments->common.outputSelect == kOutputDistributionIndexMax)
	{
		if ((arguments->common.isBenchmarkingMode) || (arguments->common.isMonteCarloMode))
		{
			fprintf(stderr, "Error: Please select a single output when in benchmarking mode or Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}

	if (numberOfYearsToRetirementArg != NULL)
	{
		int value;
		int ret = parseIntChecked(numberOfYearsToRetirementArg, &value);

		if (ret != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The number of years to retirement must be an integer.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (value < 0)
		{
			fprintf(stderr, "Error: The number of years to retirement must be non-negative.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->numberOfYearsToRetirement = value;
	}

	if (compoundedAnnualInterestRateArg != NULL)
	{
		int	ret = snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate], kCommonConstantMaxCharsPerLine, "%s", compoundedAnnualInterestRateArg);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(compoundedAnnualInterestRateArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the compounded annual interest rate from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexCompoundedAnnualInterestRate] = true;
	}

	if (totalAnnualContributionToAccountArg != NULL)
	{
		int	ret = snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount], kCommonConstantMaxCharsPerLine, "%s", totalAnnualContributionToAccountArg);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(totalAnnualContributionToAccountArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the total annual contribution to account from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexTotalAnnualContributionToAccount] = true;
	}

	if (assumedTaxRateOnInterestArg != NULL)
	{
		int	ret = snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest], kCommonConstantMaxCharsPerLine, "%s", assumedTaxRateOnInterestArg);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(assumedTaxRateOnInterestArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the assumed tax rate on interest from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexAssumedTaxRateOnInterest] = true;
	}

	if (withdrawalRateArg != NULL)
	{
		int	ret = snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate], kCommonConstantMaxCharsPerLine, "%s", withdrawalRateArg);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(withdrawalRateArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the withdrawal rate from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexWithdrawalRate] = true;
	}

	/*
	 *	Monte Carlo mode does not work with command-line parameters.
	 */
	if (distributionalArgumentGiven && (arguments->common.isMonteCarloMode))
	{
		fprintf(stderr, "Error: Monte Carlo mode cannot work with distributional arguments.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	We prioritize inputs given from CSV file.
	 */
	if (distributionalArgumentGiven && (arguments->common.isInputFromFileEnabled))
	{
		fprintf(stderr, "Warning: When reading data from an input file, input variable arguments are ignored.\n");
	}

	return kCommonConstantReturnTypeSuccess;
}

void
printHumanConsumableOutput(
	CommandLineArguments *	arguments,
	double *		outputDistributions,
	const char *		outputVariableNames[kOutputDistributionIndexMax],
	const char *		outputVariableDescriptions[kOutputDistributionIndexMax],
	double *		monteCarloOutputSamples)
{
	OutputDistributionIndex	outputSelectLowerBound;
	OutputDistributionIndex	outputSelectUpperBound;

	determineIndexRangeOfSelectedOutputs(
		arguments,
		&outputSelectLowerBound,
		&outputSelectUpperBound);

	for (OutputDistributionIndex outputSelect = outputSelectLowerBound; outputSelect < outputSelectUpperBound; outputSelect++)
	{
		/*
		 *	If in Monte Carlo mode, `pointerToOutputVariable` points to the beginning of the `monteCarloOutputSamples` array.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` is the length of the `monteCarloOutputSamples` array.
		 *	Else, it points to the entry of the `outputDistributions` to be used.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` equals 1.
		 */
		double *	pointerToValueToPrint = arguments->common.isMonteCarloMode ? monteCarloOutputSamples : &outputDistributions[outputSelect];

		for (size_t i = 0; i < arguments->common.numberOfMonteCarloIterations; ++i)
		{
			printf("%s %s is $%.2lf.\n", outputVariableDescriptions[outputSelect], outputVariableNames[outputSelect], *pointerToValueToPrint);
			pointerToValueToPrint++;
		}
	}

	return;
}

void
populateAndPrintJSONVariables(
	CommandLineArguments *	arguments,
	double *		inputVariables[kInputDistributionIndexMax],
	const char *		inputVariableDescriptions[kOutputDistributionIndexMax],
	double *		outputDistributions,
	const char *		outputVariableDescriptions[kOutputDistributionIndexMax],
	double *		monteCarloOutputSamples)
{
	JSONVariable		jsonVariables[kInputDistributionIndexMax + kOutputDistributionIndexMax];
	OutputDistributionIndex	outputSelectLowerBound;
	OutputDistributionIndex	outputSelectUpperBound;
	size_t 			jsonVariableIndex = 0;

	determineIndexRangeOfSelectedOutputs(
		arguments,
		&outputSelectLowerBound,
		&outputSelectUpperBound);

	for (size_t i = 0; i < kInputDistributionIndexMax; i++)
	{
		snprintf(jsonVariables[jsonVariableIndex].variableSymbol, kCommonConstantMaxCharsPerJSONVariableSymbol, "inputVariables[%zu]", i);
		snprintf(jsonVariables[jsonVariableIndex].variableDescription, kCommonConstantMaxCharsPerJSONVariableDescription, "%s", inputVariableDescriptions[i]);
		jsonVariables[jsonVariableIndex].values = (JSONVariablePointer){ .asDouble = inputVariables[i] };
		jsonVariables[jsonVariableIndex].type = kJSONVariableTypeDouble;
		jsonVariables[jsonVariableIndex].size = 1;
		jsonVariableIndex++;
	}

	for (OutputDistributionIndex outputSelect = outputSelectLowerBound; outputSelect < outputSelectUpperBound; outputSelect++)
	{
		/*
		 *	If in Monte Carlo mode, `pointerToOutputVariable` points to the beginning of the `monteCarloOutputSamples` array.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` is the length of the `monteCarloOutputSamples` array.
		 *	Else, it points to the entry of the `outputDistributions` to be used.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` equals 1.
		 */
		double *	pointerToOutputVariable = arguments->common.isMonteCarloMode ? monteCarloOutputSamples : &outputDistributions[outputSelect];

		snprintf(jsonVariables[jsonVariableIndex].variableSymbol, kCommonConstantMaxCharsPerJSONVariableSymbol, "outputDistributions[%u]", outputSelect);
		snprintf(jsonVariables[jsonVariableIndex].variableDescription, kCommonConstantMaxCharsPerJSONVariableDescription, "%s", outputVariableDescriptions[outputSelect]);
		jsonVariables[jsonVariableIndex].values = (JSONVariablePointer){ .asDouble = pointerToOutputVariable };
		jsonVariables[jsonVariableIndex].type = kJSONVariableTypeDouble;
		jsonVariables[jsonVariableIndex].size = arguments->common.numberOfMonteCarloIterations;
		jsonVariableIndex++;
	}

	printJSONVariables(
		&jsonVariables[outputSelectLowerBound],
		kInputDistributionIndexMax + outputSelectUpperBound - outputSelectLowerBound,
		"Input and output variables");

	return;
}

CommonConstantReturnType
prepareCSVInputVariables(
	CommandLineArguments *	arguments,
	double *		CSVInputVariables[kInputDistributionIndexMax])
{
	const char *		inputCSVHeaders[] =
				{
					"total_annual_contribution",
					"compounded_annual_interest_percentage",
					"withdrawal_rate_percentage",
					"assumed_tax_rate_on_interest_percentage"
				};

	int numberOfYearsToRetirement = arguments->numberOfYearsToRetirement;

	for (size_t i = 0; i < numberOfYearsToRetirement; i++)
	{
		double 				tempInputVariables[kInputDistributionIndexMax];
		CommonConstantReturnType	ret;

		ret = readInputDoubleDistributionsFromCSV(
				arguments->common.inputFilePath,
				inputCSVHeaders,
				tempInputVariables,
				kInputDistributionIndexMax);

		if (ret != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: Could not read from input CSV file \"%s\".\n", arguments->common.inputFilePath);

			return kCommonConstantReturnTypeError;
		}

		for (size_t j = 0; j < kInputDistributionIndexMax; j++)
		{
			CSVInputVariables[j][i] = tempInputVariables[j];
		}

	}

	return kCommonConstantReturnTypeSuccess;
}
