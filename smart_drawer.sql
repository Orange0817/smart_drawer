-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- 主機： 127.0.0.1:3307
-- 產生時間： 2026-10-11 10:17:45
-- 伺服器版本： 10.4.32-MariaDB
-- PHP 版本： 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- 資料庫： `smart_drawer`
--

-- --------------------------------------------------------

--
-- 資料表結構 `rfid_cards`
--

CREATE TABLE `rfid_cards` (
  `id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `card_uid` varchar(32) NOT NULL,
  `card_name` varchar(50) DEFAULT '門禁卡',
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `rfid_cards`
--

INSERT INTO `rfid_cards` (`id`, `user_id`, `card_uid`, `card_name`, `created_at`) VALUES
(4, 1, 'BB BD 4B 63', '卡片', '2026-10-05 10:02:06'),
(5, 1, '46 0D 11 F9', '磁扣', '2026-10-05 10:02:34'),
(6, 2, 'B1 EA B8 3C', '我的卡片', '2026-10-05 10:05:07'),
(7, 3, '75 26 AF 74', '我的卡片', '2026-10-05 10:05:59'),
(11, 6, 'BB BD 4B 63', '我的卡片01', '2026-10-06 09:18:17'),
(14, 9, 'BB BD 4B 63', '我的卡片12', '2026-10-07 10:47:21'),
(15, 10, 'BB BD 4B 63', '我的卡片123', '2026-10-07 10:48:56'),
(16, 11, 'BB BD 4B 63', '我的卡片1', '2026-10-07 10:51:25');

-- --------------------------------------------------------

--
-- 資料表結構 `users`
--

CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `username` varchar(50) NOT NULL,
  `password` varchar(255) NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `users`
--

INSERT INTO `users` (`id`, `username`, `password`, `created_at`) VALUES
(1, 'test', '$2y$10$l6ZZH8B3uJcSxXNMzLWQteDTGdqhUcvX2n/FEXMhDTGa3d0MirbG2', '2026-10-05 09:37:33'),
(2, 'hima', '$2y$10$TdoyEi5c9bv.q9zmre8JRejgsMbUmIB5J4nDk08wrYfMhk4I4iLw.', '2026-10-05 10:04:24'),
(3, 'deerufin', '$2y$10$jvLbte..CQ8IL22A05YQVunIJOa8OoLzKwVBse7.eR/kKvqz9KRwq', '2026-10-05 10:05:35'),
(4, 'ry', '$2y$10$4xfwtweUApZWWsiJbqjCt.JBa81Bx/m/X8t6cNhUz7yFkZ8ZdgbTy', '2026-10-06 09:02:56'),
(5, 'ryan', '$2y$10$rHfqzndOkFJXJ4H/fytfV.29TRPjU7Orm5YMY9bLFuLtzjBVj207e', '2026-10-06 09:10:50'),
(6, 'ryan02', '$2y$10$kwUMOgcNn8zZDUBQgtDf0OXJqCmRBPaCi1/BMa3JCxVNkIWe.HkhG', '2026-10-06 09:17:55'),
(7, 'user', '$2y$10$1FBXnWdF/xub3J4sO9wFDuFpa0NrJlHLtm489PHdnbgZKfmlFzhiq', '2026-10-07 10:40:15'),
(8, 'user12', '$2y$10$2ZJfs6T7hfGE9H0kTuwjp.WSZzPEgbAFbWK9uXNhlYwBjv6dgGwbC', '2026-10-07 10:43:15'),
(9, 'user123', '$2y$10$G1T14vYb8v9eDWCHU6Tfm..hRG6pDjmoNiiKiZpNJgx8IuJq4l4Pq', '2026-10-07 10:46:56'),
(10, 'user1234', '$2y$10$3HkEUZs.8INvNkHhN53Qt.aOXp1jD7FGiuIOYRYSFHtCuYS6UgniS', '2026-10-07 10:48:32'),
(11, 'testtt', '$2y$10$30hy/9S.Hay6Tmza4fNdputFMBw5etwS3N7qeVDvZrc2SASsQQPsO', '2026-10-07 10:51:06');

--
-- 已傾印資料表的索引
--

--
-- 資料表索引 `rfid_cards`
--
ALTER TABLE `rfid_cards`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `unique_user_card` (`user_id`,`card_uid`);

--
-- 資料表索引 `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `username` (`username`);

--
-- 在傾印的資料表使用自動遞增(AUTO_INCREMENT)
--

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `rfid_cards`
--
ALTER TABLE `rfid_cards`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=17;

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `users`
--
ALTER TABLE `users`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;

--
-- 已傾印資料表的限制式
--

--
-- 資料表的限制式 `rfid_cards`
--
ALTER TABLE `rfid_cards`
  ADD CONSTRAINT `rfid_cards_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
