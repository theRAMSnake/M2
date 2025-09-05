import { DatabaseService } from '../../storage/database';
import { generateId } from '../../utils/routeHelpers';
import { ChoresService } from '../chores/service';
import crypto from 'crypto';

export interface CoinBalance {
  red: number;
  blue: number;
  purple: number;
  yellow: number;
  green: number;
}

export interface Pool {
  id: string;
  name: string;
  currentAmount: number;
  maxAmount: number;
}

export interface CoinHistoryEntry {
  timestamp: string;
  cause: string;
  color: string;
  value: number;
}

export interface Modifier {
  id: string;
  name: string;
  color: string; // 'random' or specific color
  value: number; // positive = produces, negative = consumes
}

export interface DisciplineData {
  coins: CoinBalance;
  pools: Pool[];
  nextCouponPrice: number; // Price of the next coupon to be earned
  nextCouponColor: string; // Color of the next coupon to be earned
  workBurden: number; // Current work burden counter
  dailyWorkBurdenIncrement: number; // Daily increment amount (constant)
  workBurdenBaseline: number; // Previous day's starting work burden value
  notebook: string; // Simple text notebook content
}

export class DisciplineService {
  private db: DatabaseService;
  private readonly DISCIPLINE_PATH = '/snake/discipline.cb';
  private readonly HISTORY_PATH = '/snake/discipline.history';
  private readonly MODIFIERS_PATH = '/snake/discipline.modifiers';
  private readonly COIN_COLORS = ['red', 'blue', 'purple', 'yellow', 'green'] as const;
  private readonly INITIAL_COUPON_PRICE = 4;
  private readonly MAX_HISTORY_ENTRIES = 100;

  // Map chore colors to coin colors
  private mapChoreColorToCoinColor(choreColor: string): string {
    const colorMapping: Record<string, string> = {
      'red': 'red',
      'pink': 'red',      // Map pink to red
      'purple': 'purple',
      'blue': 'blue',
      'green': 'green',
      'yellow': 'yellow',
      'orange': 'yellow', // Map orange to yellow
      'brown': 'yellow',  // Map brown to yellow
      'grey': 'blue',     // Map grey to blue
    };
    
    return colorMapping[choreColor] || 'blue'; // Default to blue if color not found
  }

  constructor() {
    this.db = DatabaseService.getInstance();
  }

  // Centralized function for all coin modifications
  private async modifyCoins(data: DisciplineData, color: string, amount: number, cause: string): Promise<CoinHistoryEntry[]> {
    const historyEntries: CoinHistoryEntry[] = [];
    
    // Helper function to modify a single coin and return history entry
    const modifySingleCoin = (coinColor: string, coinAmount: number): CoinHistoryEntry | null => {
      const currentAmount = (data.coins as any)[coinColor];
      const newAmount = Math.max(0, currentAmount + coinAmount); // Ensure coins never go negative
      const actualChange = newAmount - currentAmount;
      
      if (actualChange !== 0) {
        (data.coins as any)[coinColor] = newAmount;
        return {
          timestamp: new Date().toISOString(),
          cause,
          color: coinColor,
          value: actualChange
        };
      }
      
      return null;
    };
    
    if (color === 'random') {
      // For random modifications, apply to each coin individually
      for (let i = 0; i < Math.abs(amount); i++) {
        const coinChange = amount > 0 ? 1 : -1;
        let entry: CoinHistoryEntry | null = null;
        
        if (coinChange > 0) {
          // Adding coins - just pick a random color
          const randomColor = this.generateRandomColor();
          entry = modifySingleCoin(randomColor, coinChange);
        } else {
          // Subtracting coins - find a color that has coins available
          const availableColors = this.COIN_COLORS.filter(coinColor => (data.coins as any)[coinColor] > 0);
          
          if (availableColors.length > 0) {
            // Pick a random color from available ones
            const randomIndex = Math.floor(Math.random() * availableColors.length);
            const selectedColor = availableColors[randomIndex];
            entry = modifySingleCoin(selectedColor, coinChange);
          }
          // If no coins available, entry remains null (no change)
        }
        
        if (entry) {
          historyEntries.push(entry);
        }
      }
    } else if (this.COIN_COLORS.includes(color as any)) {
      const entry = modifySingleCoin(color, amount);
      if (entry) {
        historyEntries.push(entry);
      }
    }
    
    return historyEntries;
  }

  private async loadDisciplineData(): Promise<DisciplineData> {
    try {
      const data = await this.db.readDocument(this.DISCIPLINE_PATH);
      if (data) {
        const disciplineData = data as DisciplineData;
        // Ensure nextCouponColor is set (for backward compatibility)
        if (!disciplineData.nextCouponColor) {
          disciplineData.nextCouponColor = this.determineNextCouponColor(disciplineData);
        }
        // Ensure work burden fields are set (for backward compatibility)
        if (typeof disciplineData.workBurden !== 'number') {
          disciplineData.workBurden = 0;
        }
        if (typeof disciplineData.dailyWorkBurdenIncrement !== 'number') {
          disciplineData.dailyWorkBurdenIncrement = 320;
        }
        if (typeof disciplineData.workBurdenBaseline !== 'number') {
          disciplineData.workBurdenBaseline = 0;
        }
        if (typeof disciplineData.notebook !== 'string') {
          disciplineData.notebook = '';
        }
        return disciplineData;
      }
    } catch (error) {
      // Document doesn't exist, create default structure
    }

    // Return default structure
    return {
      coins: {
        red: 0,
        blue: 0,
        purple: 0,
        yellow: 0,
        green: 0
      },
      pools: [],
      nextCouponPrice: this.INITIAL_COUPON_PRICE,
      nextCouponColor: this.generateRandomColor(),
      workBurden: 0,
      dailyWorkBurdenIncrement: 320,
      workBurdenBaseline: 0,
      notebook: ''
    };
  }

  // Determine the next coupon color (always random, but never the same as previous)
  private determineNextCouponColor(data: DisciplineData): string {
    const availableColors = this.COIN_COLORS.filter(color => color !== data.nextCouponColor);
    const randomBytes = crypto.randomBytes(1);
    return availableColors[randomBytes[0] % availableColors.length];
  }

  private async saveDisciplineData(data: DisciplineData): Promise<void> {
    await this.db.writeDocument(this.DISCIPLINE_PATH, data);
  }

  // Hardware random generator using crypto.randomBytes
  private generateRandomColor(): string {
    const randomBytes = crypto.randomBytes(1);
    const randomIndex = randomBytes[0] % this.COIN_COLORS.length;
    return this.COIN_COLORS[randomIndex];
  }

  // Fill a random pool that's not yet full
  private fillRandomPool(data: DisciplineData): string | undefined {
    // Get pools that are not yet full
    const availablePools = data.pools.filter(pool => pool.currentAmount < pool.maxAmount);
    
    if (availablePools.length === 0) {
      return undefined; // No pools available to fill
    }
    
    // Select a random pool from available ones
    const randomBytes = crypto.randomBytes(1);
    const selectedPool = availablePools[randomBytes[0] % availablePools.length];
    
    // Fill the pool by 1 unit
    selectedPool.currentAmount += 1;
    
    return selectedPool.name;
  }

  async getDisciplineData(): Promise<DisciplineData> {
    return await this.loadDisciplineData();
  }

  async addCoins(color: string, amount: number): Promise<CoinBalance> {
    const data = await this.loadDisciplineData();
    const historyEntries = await this.modifyCoins(data, color, amount, 'manual_add');
    
    if (historyEntries.length > 0) {
      await this.addHistoryEntries(historyEntries);
    }
    
    await this.saveDisciplineData(data);
    return data.coins;
  }

  async rewardChoreCompletion(choreColor: string, choreTitle: string): Promise<CoinBalance> {
    const data = await this.loadDisciplineData();
    const coinColor = this.mapChoreColorToCoinColor(choreColor);
    const historyEntries = await this.modifyCoins(data, coinColor, 1, `Chore completed: ${choreTitle}`);
    
    if (historyEntries.length > 0) {
      await this.addHistoryEntries(historyEntries);
    }
    
    await this.saveDisciplineData(data);
    return data.coins;
  }



  private async buyCouponInternal(data: DisciplineData): Promise<{ success: boolean; message: string; color?: string }> {
    // Check if we have enough coins for the predetermined color
    if ((data.coins as any)[data.nextCouponColor] < data.nextCouponPrice) {
      return {
        success: false,
        message: `Not enough ${data.nextCouponColor} coins to buy a coupon`
      };
    }

    // Use the predetermined color
    const selectedColor = data.nextCouponColor;

    // Deduct coins using centralized function
    const historyEntries = await this.modifyCoins(data, selectedColor, -data.nextCouponPrice, 'coupon_purchase');
    
    if (historyEntries.length > 0) {
      await this.addHistoryEntries(historyEntries);
    }

    return {
      success: true,
      message: `Coupon purchased for ${selectedColor} coins`,
      color: selectedColor
    };
  }

  async buyCoupon(): Promise<{ success: boolean; message: string; color?: string; nextColor?: string; poolFilled?: string }> {
    const data = await this.loadDisciplineData();
    
    const result = await this.buyCouponInternal(data);
    
    if (result.success) {
      // Fill a random pool that's not yet full
      const poolFilled = this.fillRandomPool(data);
      
      // Determine the next coupon color (price stays at 4)
      data.nextCouponColor = this.determineNextCouponColor(data);
      
      await this.saveDisciplineData(data);
      
      return {
        success: true,
        message: result.message,
        color: result.color,
        nextColor: data.nextCouponColor,
        poolFilled
      };
    }
    
    return result;
  }

  async createPool(name: string, maxAmount: number): Promise<Pool> {
    const data = await this.loadDisciplineData();
    
    const pool: Pool = {
      id: generateId(),
      name,
      currentAmount: 0,
      maxAmount
    };

    data.pools.push(pool);
    await this.saveDisciplineData(data);

    return pool;
  }

  async updatePool(poolId: string, updates: Partial<Pool>): Promise<Pool> {
    const data = await this.loadDisciplineData();
    
    const poolIndex = data.pools.findIndex(p => p.id === poolId);
    if (poolIndex === -1) {
      throw new Error('Pool not found');
    }

    const pool = data.pools[poolIndex];
    const updatedPool = { ...pool, ...updates, id: poolId };

    // Ensure current amount doesn't exceed max amount
    if (updatedPool.currentAmount > updatedPool.maxAmount) {
      updatedPool.currentAmount = updatedPool.maxAmount;
    }

    data.pools[poolIndex] = updatedPool;
    await this.saveDisciplineData(data);

    return updatedPool;
  }

  async deletePool(poolId: string): Promise<void> {
    const data = await this.loadDisciplineData();
    
    // Remove pool
    data.pools = data.pools.filter(p => p.id !== poolId);

    await this.saveDisciplineData(data);
  }

  async emptyPool(poolId: string): Promise<void> {
    const data = await this.loadDisciplineData();
    const pool = data.pools.find(p => p.id === poolId);
    if (pool) {
      pool.currentAmount = 0;
      await this.saveDisciplineData(data);
    }
  }

  async getPools(): Promise<Pool[]> {
    const data = await this.loadDisciplineData();
    return data.pools;
  }

  async getNextCouponPrice(): Promise<number> {
    const data = await this.loadDisciplineData();
    return data.nextCouponPrice;
  }

  // Work Burden methods
  async getWorkBurden(): Promise<number> {
    const data = await this.loadDisciplineData();
    return data.workBurden;
  }

  async decreaseWorkBurden(amount: number): Promise<{ newWorkBurden: number }> {
    const data = await this.loadDisciplineData();
    data.workBurden -= amount;
    await this.saveDisciplineData(data);
    return { newWorkBurden: data.workBurden };
  }

  async incrementWorkBurdenDaily(): Promise<void> {
    const today = new Date();
    const dayOfWeek = today.getDay(); // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
    
    // Only increment on weekdays (Monday = 1 to Friday = 5)
    if (dayOfWeek >= 1 && dayOfWeek <= 5) {
      const data = await this.loadDisciplineData();
      const oldWorkBurden = data.workBurden;
      
      // Update baseline to current value before incrementing
      data.workBurdenBaseline = data.workBurden;
      data.workBurden += data.dailyWorkBurdenIncrement;
      
      // Apply work burden deposit (only when incrementing)
      const depositEntries = await this.applyWorkBurdenDeposit(data, oldWorkBurden, data.workBurden);
      
      if (depositEntries.length > 0) {
        await this.addHistoryEntries(depositEntries);
      }
      
      await this.saveDisciplineData(data);
    }
  }

  async applyDailyModifiers(): Promise<void> {
    const data = await this.loadDisciplineData();
    const allHistoryEntries: CoinHistoryEntry[] = [];
    
    // Apply modifiers
    const modifierEntries = await this.applyModifiers(data);
    allHistoryEntries.push(...modifierEntries);
    
    // Apply work burden reward every day (not just when incrementing)
    const rewardEntries = await this.applyWorkBurdenReward(data);
    allHistoryEntries.push(...rewardEntries);
    
    // Apply chores completion reward
    const choresRewardEntries = await this.applyChoresCompletionReward(data);
    allHistoryEntries.push(...choresRewardEntries);
    
    // Save all changes at once
    if (allHistoryEntries.length > 0) {
      await this.addHistoryEntries(allHistoryEntries);
    }
    
    await this.saveDisciplineData(data);
  }

  private async applyWorkBurdenDeposit(data: DisciplineData, oldWorkBurden: number, newWorkBurden: number): Promise<CoinHistoryEntry[]> {
    const workBurdenIncrement = newWorkBurden - oldWorkBurden;
    
    // Random coins are subtracted with the work burden increment in mind
    // Each coin per 30 of work burden incremented -- reason "Workburden deposit"
    const coinsToSubtract = Math.floor(workBurdenIncrement / 30);
    
    if (coinsToSubtract > 0) {
      return await this.modifyCoins(data, 'random', -coinsToSubtract, 'Workburden deposit');
    }
    
    return [];
  }

  private async applyWorkBurdenReward(data: DisciplineData): Promise<CoinHistoryEntry[]> {
    // If wb is positive: subtract 2 purple coins, if negative: add 1 purple
    // -- reason "Workburden reward"
    if (data.workBurden > 0) {
      return await this.modifyCoins(data, 'purple', -2, 'Workburden reward');
    } else if (data.workBurden < 0) {
      return await this.modifyCoins(data, 'purple', 1, 'Workburden reward');
    }
    
    return [];
  }

  private async applyChoresCompletionReward(data: DisciplineData): Promise<CoinHistoryEntry[]> {
    // If there are no chores left (all done) - add +2 random coins
    // -- reason "Chores completion reward"
    try {
      const choresService = new ChoresService();
      const allChores = await choresService.getChores('snake');
      
      // Check if there are any incomplete chores
      const incompleteChores = allChores.filter(chore => !chore.is_done);
      
      if (incompleteChores.length === 0 && allChores.length > 0) {
        // All chores are done and there are chores to begin with
        return await this.modifyCoins(data, 'random', 2, 'Chores completion reward');
      }
    } catch (error) {
      console.error('Error checking chores completion:', error);
    }
    
    return [];
  }

  // Notebook methods
  async getNotebook(): Promise<string> {
    const data = await this.loadDisciplineData();
    return data.notebook;
  }

  async updateNotebook(content: string): Promise<void> {
    const data = await this.loadDisciplineData();
    data.notebook = content;
    await this.saveDisciplineData(data);
  }

  // History methods
  private async loadHistory(): Promise<CoinHistoryEntry[]> {
    try {
      const data = await this.db.readDocument(this.HISTORY_PATH);
      if (data && Array.isArray(data)) {
        return data as CoinHistoryEntry[];
      }
    } catch (error) {
      // History doesn't exist, return empty array
    }
    return [];
  }

  private async saveHistory(history: CoinHistoryEntry[]): Promise<void> {
    await this.db.writeDocument(this.HISTORY_PATH, history);
  }

  private async addHistoryEntry(cause: string, color: string, value: number): Promise<void> {
    const entry: CoinHistoryEntry = {
      timestamp: new Date().toISOString(),
      cause,
      color,
      value
    };
    await this.addHistoryEntries([entry]);
  }

  private async addHistoryEntries(entries: CoinHistoryEntry[]): Promise<void> {
    if (entries.length === 0) return;
    
    const history = await this.loadHistory();
    
    // Add new entries at the beginning (most recent first)
    history.unshift(...entries);
    
    // Keep only the first MAX_HISTORY_ENTRIES entries (most recent)
    if (history.length > this.MAX_HISTORY_ENTRIES) {
      history.splice(this.MAX_HISTORY_ENTRIES);
    }
    
    await this.saveHistory(history);
  }

  // Modifier methods
  private async loadModifiers(): Promise<Modifier[]> {
    try {
      const data = await this.db.readDocument(this.MODIFIERS_PATH);
      if (data && Array.isArray(data)) {
        return data as Modifier[];
      }
    } catch (error) {
      // Modifiers don't exist, return empty array
    }
    return [];
  }

  private async saveModifiers(modifiers: Modifier[]): Promise<void> {
    await this.db.writeDocument(this.MODIFIERS_PATH, modifiers);
  }

  private async applyModifiers(data: DisciplineData): Promise<CoinHistoryEntry[]> {
    const modifiers = await this.loadModifiers();
    const allHistoryEntries: CoinHistoryEntry[] = [];
    
    for (const modifier of modifiers) {
      const entries = await this.modifyCoins(data, modifier.color, modifier.value, `modifier_${modifier.name}`);
      allHistoryEntries.push(...entries);
    }
    
    return allHistoryEntries;
  }

  async getModifiers(): Promise<Modifier[]> {
    return await this.loadModifiers();
  }

  async createModifier(name: string, color: string, value: number): Promise<Modifier> {
    const modifiers = await this.loadModifiers();
    const modifier: Modifier = {
      id: generateId(),
      name,
      color,
      value
    };
    modifiers.push(modifier);
    await this.saveModifiers(modifiers);
    return modifier;
  }

  async updateModifier(modifierId: string, updates: Partial<Omit<Modifier, 'id'>>): Promise<Modifier> {
    const modifiers = await this.loadModifiers();
    const modifierIndex = modifiers.findIndex(modifier => modifier.id === modifierId);
    if (modifierIndex === -1) {
      throw new Error('Modifier not found');
    }
    
    modifiers[modifierIndex] = { ...modifiers[modifierIndex], ...updates };
    await this.saveModifiers(modifiers);
    return modifiers[modifierIndex];
  }

  async deleteModifier(modifierId: string): Promise<void> {
    const modifiers = await this.loadModifiers();
    const filteredModifiers = modifiers.filter(modifier => modifier.id !== modifierId);
    await this.saveModifiers(filteredModifiers);
  }

}
