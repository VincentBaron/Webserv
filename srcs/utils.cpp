/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vincentbaron <vincentbaron@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/02 17:46:20 by vincentbaro       #+#    #+#             */
/*   Updated: 2022/03/02 17:47:34 by vincentbaro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

void errDie(std::string mssg)
{
	std::cerr << mssg << std::endl;
	exit(EXIT_FAILURE);
}