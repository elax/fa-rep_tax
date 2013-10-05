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

INSERT INTO `0_tax_rep` (`tax_rep_id`, `code`, `description`, `basis`, `liability_account`, `bank_account`, `inactive`) VALUES
(1, 'BAS_AU', 'Business Activity Statement', 'Accrual', '21426', '8', 0),
(2, 'GST_NZ', 'GST Return (GST 101A)', 'Accrual', '21426', '8', 0);

-- --------------------------------------------------------

INSERT INTO `0_tax_rep_line` (`tax_rep_id`, `code`, `description`, `line_type`, `calculation`, `code_basis`, `sequence`) VALUES
(1, 'G1', 'GST sales total', 'tax_code', '', 'SalesNet', 1),
(1, 'G2', 'Export sales total', 'tax_code', '', 'SalesNet', 2),
(1, 'G3', 'Other GST free sales', 'tax_code', '', 'SalesNet', 3),
(1, 'G10', 'Capital Purchases', 'tax_code', '', 'PurchaseGross', 4),
(1, 'G11', 'Non-capital purchases', 'tax_code', '', 'PurchaseGross', 5),
(1, 'W1', 'Total salaries and wages', 'account_code', '', '', 6),
(1, 'W2', 'Amount withheld from W1', 'account_code', '', '', 7),
(1, 'W4', 'Amount withheld no ABN', 'account_code', '', '', 8),
(1, 'W3', 'Other amounts withheld', 'account_code', '', '', 9),
(1, 'W5', 'Total withheld', 'compute', 'W1 + W2 + W3 + W4', '', 10),
(1, 'T1', 'PAYG instalment income', 'account_code', '', '', 11),
(1, 'T11', 'Instalment tax', 'compute', 'T1 * T3 / 100', '', 14),
(1, 'T3', 'Rate % or Varied rate', 'compute', '6.75', '', 13),
(1, 'T4', 'Variation code', 'user', '', '', 15),
(1, 'F1', 'Fringe benefits', 'account_code', '', '', 16),
(1, 'F4', 'Variation code', 'user', '', '', 17),
(1, '1A', 'GST on sales', 'tax_code', '', 'SalesTax', 18),
(1, '1C', 'Wine equalisation tax', 'tax_code', '', 'SalesTax', 19),
(1, '1E', 'Luxury car tax', 'tax_code', '', 'SalesTax', 20),
(1, '4', 'PAYG tax withheld', 'account_code', '', '', 21),
(1, '5A', 'PAYG income tax instalment', 'account_code', '', '', 22),
(1, '6A', 'Fringe benefit tax', 'account_code', '', '', 23),
(1, '7', 'Deferred company instalment', 'account_code', '', '', 24),
(1, '8A', 'Total owed to ATO', 'compute', '{1A} + {1C} + {1E} + {4} + {5A} + {6A} + {7}', '', 25),
(1, '1B', 'GST on purchases', 'tax_code', '', 'PurchaseTax', 26),
(1, '1D', 'Wine equalisation tax refund', 'tax_code', '', 'PurchaseTax', 27),
(1, '1F', 'Luxury car tax refund', 'tax_code', '', 'PurchaseTax', 28),
(1, '5B', 'PAYG tax refund', 'account_code', '', '', 29),
(1, '6B', 'Fringe benefit tax refund', 'account_code', '', '', 30),
(1, '8B', 'Total owed to you', 'compute', '{1B} + {1D} + {1F} + {5B} + {6B}', '', 31),
(1, '9', 'Total to pay/refund', 'compute', '{8A} - {8B}', '', 32),
(2, '5', 'Total Sales', 'tax_code', '', 'SalesGross', 1),
(2, '8', 'GST on Sales', 'compute', '{7} * 3 / 23', '', 4),
(2, '9', 'GST adjustments', 'account_code', '', '', 5),
(2, '6', 'Zero rated sales', 'tax_code', '', 'SalesGross', 2),
(2, '11', 'Total Purchases', 'tax_code', '', 'PurchaseGross', 7),
(2, '7', 'Subtract zero rated from total sales', 'compute', '{5} - {6}', '', 3),
(2, '10', 'Total GST on sales', 'compute', '{8} + {9}', '', 6),
(2, '12', 'GST on purchases', 'compute', '{11} * 3 / 23', '', 8),
(2, '13', 'GST credit adjustments', 'account_code', '', '', 9),
(2, '14', 'Total GST on purchases', 'compute', '{12} + {13}', '', 10),
(2, '15', 'GST payable', 'compute', '{10} - {14}', '', 11);
