/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: soksak <soksak@42istanbul.com.tr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 23:42:39 by soksak            #+#    #+#             */
/*   Updated: 2025/08/13 23:59:14 by soksak           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTREGISTRY_HPP
#define CLIENTREGISTRY_HPP

#include <iostream>

class User
{
	private:
		int _fd;
		std::string _nickName;
		std::string _userName;
		std::string _realName;
		bool _gotNick;
		bool _gotUser;
		bool _registered;

	public:

		// Constructor
		User(int &fd);
		// Destructor
		~User();

		int fd() const;
		const std::string &getNick() const;
		const std::string &getUser() const;
		const std::string &getRealName() const;

		void setNick(const std::string &n);
		void setUser(const std::string &u, const std::string &real);

		bool hasNick() const;
		bool hasUser() const;

		bool readyToRegister() const;
		bool isRegistered() const;
		void markRegistered();

};

#endif
