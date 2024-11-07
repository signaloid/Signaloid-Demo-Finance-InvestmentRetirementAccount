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

#include "kernel.h"
#include "utilities.h"


double
calculateFutureValueTaxed(
	int		numberOfYearsToRetirement,
	double *	inputVariables[kInputDistributionIndexMax])
{
	double *	totalAnnualContributionToAccount = inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount];
	double *	compoundedAnnualInterestRate = inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate];
	double *	assumedTaxRateOnInterest = inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest];
	double		futureValue = 0.0;
	double		currentYearValueIncreaseRate;

	for (int i = 0; i < numberOfYearsToRetirement; i++)
	{
		currentYearValueIncreaseRate =
			(compoundedAnnualInterestRate[i] / 100) *
			(1.0 - (assumedTaxRateOnInterest[i] / 100));

		futureValue =
			(futureValue + totalAnnualContributionToAccount[i]) *
			(1.0 + currentYearValueIncreaseRate);
	}

	return futureValue;
}

double
calculateFutureValueTaxedWithdrawal(
	int		numberOfYearsToRetirement,
	double *	inputVariables[kInputDistributionIndexMax])
{
	double *	totalAnnualContributionToAccount = inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount];
	double *	compoundedAnnualInterestRate = inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate];
	double *	withdrawalRate = inputVariables[kInputDistributionIndexWithdrawalRate];
	double		futureValue = 0.0;
	double		currentYearValueIncreaseRate;

	for (int i = 0; i < numberOfYearsToRetirement; i++)
	{
		currentYearValueIncreaseRate = (compoundedAnnualInterestRate[i] / 100);

		futureValue =
			(futureValue + totalAnnualContributionToAccount[i] * (1.0 - (withdrawalRate[i] / 100))) *
			(1.0 + currentYearValueIncreaseRate);
	}

	return futureValue;
}

void
calculateOutput(
	CommandLineArguments *	arguments,
	size_t			numberOfYearsToRetirement,
	double *		inputVariables[kInputDistributionIndexMax],
	double *		outputDistributions)
{
	bool	calculateAllOutputs = (arguments->common.outputSelect == kOutputDistributionIndexMax);

	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputDistributionIndexFutureValueTaxed))
	{
		outputDistributions[kOutputDistributionIndexFutureValueTaxed] = calculateFutureValueTaxed(
											numberOfYearsToRetirement,
											inputVariables);
	}

	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputDistributionIndexFutureValueTaxedWithdrawal))
	{
		outputDistributions[kOutputDistributionIndexFutureValueTaxedWithdrawal] = calculateFutureValueTaxedWithdrawal(
												numberOfYearsToRetirement,
												inputVariables);
	}

	return;
}
