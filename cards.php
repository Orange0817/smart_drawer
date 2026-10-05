<?php
session_start();
header('Content-Type: application/json');
require 'db.php';

if (!isset($_SESSION['user_id'])) {
    http_response_code(401);
    echo json_encode(['error' => '未登入']);
    exit;
}

$user_id = $_SESSION['user_id'];
$method = $_SERVER['REQUEST_METHOD'];

// 1. 取得該使用者的所有卡片
if ($method === 'GET') {
    $stmt = $pdo->prepare('SELECT id, card_uid, card_name, created_at FROM rfid_cards WHERE user_id = ? ORDER BY id DESC');
    $stmt->execute([$user_id]);
    echo json_encode(['cards' => $stmt->fetchAll()]);
    exit;
}

// 2. 新增卡片
if ($method === 'POST') {
    $data = json_decode(file_get_contents('php://input'), true);
    $card_uid = trim($data['card_uid'] ?? '');
    $card_name = trim($data['card_name'] ?? '我的卡片');

    if (empty($card_uid)) {
        echo json_encode(['success' => false, 'message' => 'UID 不得為空']);
        exit;
    }

    // 檢查卡片上限是否超過 5 張
    $stmt = $pdo->prepare('SELECT COUNT(*) FROM rfid_cards WHERE user_id = ?');
    $stmt->execute([$user_id]);
    if ($stmt->fetchColumn() >= 5) {
        echo json_encode(['success' => false, 'message' => '已達卡片儲存上限（最多 5 張）']);
        exit;
    }

    try {
        $stmt = $pdo->prepare('INSERT INTO rfid_cards (user_id, card_uid, card_name) VALUES (?, ?, ?)');
        $stmt->execute([$user_id, $card_uid, $card_name]);
        echo json_encode(['success' => true]);
    } catch (PDOException $e) {
        echo json_encode(['success' => false, 'message' => '此卡片已新增過']);
    }
    exit;
}

// 3. 編輯卡片名稱 (新增功能)
if ($method === 'PUT') {
    $data = json_decode(file_get_contents('php://input'), true);
    $card_id = intval($data['id'] ?? 0);
    $card_name = trim($data['card_name'] ?? '');

    if (empty($card_name)) {
        echo json_encode(['success' => false, 'message' => '卡片名稱不可為空']);
        exit;
    }

    $stmt = $pdo->prepare('UPDATE rfid_cards SET card_name = ? WHERE id = ? AND user_id = ?');
    $stmt->execute([$card_name, $card_id, $user_id]);
    
    echo json_encode(['success' => true, 'message' => '卡片名稱更新成功']);
    exit;
}

// 4. 刪除卡片
if ($method === 'DELETE') {
    $data = json_decode(file_get_contents('php://input'), true);
    $card_id = intval($data['id'] ?? 0);

    $stmt = $pdo->prepare('DELETE FROM rfid_cards WHERE id = ? AND user_id = ?');
    $stmt->execute([$card_id, $user_id]);
    echo json_encode(['success' => true]);
    exit;
}