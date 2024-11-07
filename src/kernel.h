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

#include "utilities.h"


/**
 *	@brief	Calculate taxed future value.
 *
 *	@param	numberOfYearsToRetirement	: Number of years to retirement.
 *	@param	inputVariables			: The input variables.
 *	@return					: Taxed future value.
 */
double	calculateFutureValueTaxed(
		int		numberOfYearsToRetirement,
		double *	inputVariables[kInputDistributionIndexMax]);

/**
 *	@brief	Calculate taxed future value assuming a withdrawal rate.
 *
 *	@param	numberOfYearsToRetirement	: Number of years to retirement.
 *	@param	inputVariables			: The input variables.
 *	@return					: Tax free future value.
 */
double	calculateFutureValueTaxedWithdrawal(
		int		numberOfYearsToRetirement,
		double *	inputVariables[kInputDistributionIndexMax]);

/**
 *	@brief	Calculate output.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	numberOfYearsToRetirement	: Number of years to retirement.
 *	@param	inputVariables			: The input variables.
 *	@param	outputDistributions		: The output variables.
 */
void	calculateOutput(
		CommandLineArguments *	arguments,
		size_t			numberOfYearsToRetirement,
		double *		inputVariables[kInputDistributionIndexMax],
		double *		outputDistributions);
