/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 17:46:20 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/07 10:21:06 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <stdlib.h>

void err_n_die(std::string mssg)
{
	std::cerr << "Error: " << mssg << std::endl;
	exit(EXIT_FAILURE);
}