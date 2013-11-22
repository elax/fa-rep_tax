DROP TABLE IF EXISTS `0_tax_rep`;
DROP TABLE IF EXISTS `0_tax_rep_line`;
DROP TABLE IF EXISTS `0_tax_rep_line_accounts`;

CREATE TABLE `0_tax_rep` (
  `tax_rep_id` int(11) NOT NULL AUTO_INCREMENT,
  `code` varchar(50) NOT NULL DEFAULT '',
  `description` varchar(100) NOT NULL DEFAULT '',
  `basis` varchar(10) NOT NULL DEFAULT 0, -- accrual,  payment
  `balancing_journal` tinyint(1) NOT NULL DEFAULT 0,
  `liability_account` varchar(15) NOT NULL DEFAULT '', -- for total tax owed
  `tax_payment` tinyint(1) NOT NULL DEFAULT 0,
  `bank_account` varchar(15) NOT NULL DEFAULT '', -- for tax payment
  `custom` TEXT NOT NULL DEFAULT '', -- for coutnry specific attributes
  `inactive` tinyint(1) NOT NULL DEFAULT 0,
  PRIMARY KEY (`tax_rep_id`),
  KEY `code` (`code`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

CREATE TABLE `0_tax_rep_line` (
  `tax_rep_line_id` int(11) NOT NULL AUTO_INCREMENT,
  `tax_rep_id` int(11) NOT NULL,
  `code` varchar(50) NOT NULL DEFAULT '',
  `description` varchar(100) NOT NULL DEFAULT '',
  `line_type` varchar(15) NOT NULL DEFAULT 0, -- tax_code, account_code, compute
  `calculation` varchar(255) NOT NULL DEFAULT '',
  `code_basis` varchar(20) NOT NULL DEFAULT 0, -- SalesGross,SalesNet,SalesTax,PurchaseGross,PurchaseNet,PurchaseTax
  `sequence` int(11) NOT NULL,
  PRIMARY KEY (`tax_rep_line_id`),
  KEY `tax_rep_id` (`tax_rep_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

CREATE TABLE `0_tax_rep_line_accounts` (
  `tax_rep_line_account_id` int(11) NOT NULL AUTO_INCREMENT,
  `tax_rep_line_id` int(11) NOT NULL,
  `code` varchar(60) NOT NULL DEFAULT '',
  PRIMARY KEY (`tax_rep_line_account_id`),
  KEY `tax_rep_line_id` (`tax_rep_line_id`)
) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;

